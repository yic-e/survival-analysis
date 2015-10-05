#include "user_constant_model.h"
#include "global_constant_model.h"
#include "piecewise_constant_model.h"
#include "construct_feature_model.h"
#include "data_io.h"
#include <iostream>
using namespace std;
void PiecewiseConstantModel::initParams(){                                              

  // feature-based parameters                                                      
  lr_lambda = _config["lr_lambda"].as<double>();
  lr_lambda_u = _config["lr_lambda_u"].as<double>();              
  momentum  = _config["momentum"].as<double>();                   
  max_iter  = _config["max_iter"].as<int>();
  cerr <<"=====lr_lambda = "<<lr_lambda<<endl                                      
    <<"=====lr_lambda_u= "<<lr_lambda_u<<endl                                      
    <<"=====momentum = "<<momentum<<endl;
  // weights for features                                                          
  //W  = SparseVector::rand_init(num_feature);                                     
}  
double PiecewiseConstantModel::evalLoglik(vector<DataPoint> & data){
  double loglik = 0.0;                                                          
  unordered_map<long, int> perUserCount;                                        
  unordered_map<long, double> perUserLik;                                       
  long n_session =  data.size();
  double sum_loglik =0.0;
  for(int i = 0 ; i < (int)data.size(); i++){                                   
    DataPoint &_point = data[i];                                                
    long uid = _point.uid;
    assert(lambda_u.find(uid) != lambda_u.end()); 
    double y = _point.y;
    int bin = min(NUM_BIN-1, (int)(y/(double)BIN_WIDTH));
    double _loglik = log(lambda_u[uid] + lambda + lambda_bin[bin])
      - y * (lambda + lambda_u[uid]);
    for(int b = 0 ; b < bin ; b++){
      _loglik -= lambda_bin[b] * BIN_WIDTH;
    }
    assert(y >= bin * BIN_WIDTH);
    _loglik -= (y - bin * BIN_WIDTH) * lambda_bin[bin];
    perUserCount[uid] ++;                                                       
    perUserLik[uid] += _loglik;                                                 
    //    loglik += _loglik;                                                    
  }                                                                             
  for(auto iter : perUserCount){                                                
    loglik += perUserLik[iter.first]/iter.second;
    sum_loglik += perUserLik[iter.first];
  }                                                                             
  cout << "evaluated_user = "<< perUserCount.size()<<endl;
  //  cerr <<"=========Avg Perp = "<<exp(-sum_loglik/n_session);
  //  cerr <<"=========User-Avg Perp = "<<exp(-loglik/(double)perUserCount.size());
  //return exp(-loglik/(double)perUserCount.size());                              
  return exp(-sum_loglik/n_session);       
}

int PiecewiseConstantModel::train(const UserContainer *data){
  initParams();
  ConstructFeatureModel ctrFeature(NO_FEATURE);
  ctrFeature.setData(_train_data, _test_data);
  assert(_train_data != nullptr);                                                  
  assert(_test_data != nullptr);
  ctrFeature.train(_train_data);                                                   
  vector<DataPoint> train_data = ctrFeature.getTrainSet();
  vector<DataPoint> test_data = ctrFeature.getTestSet();
  cerr <<"=======# train_sessions = "<<train_data.size()<<endl
    <<"=======# test_sessions = "<<test_data.size()<<endl;

  for(auto iter = data->begin();                                                   
      iter != data->end(); ++iter){   
    long uid = iter->first;
    lambda_u[uid] = EPS_LAMBDA;
    d_lambda_u[uid] = 0.0;
  }
  lambda = d_lambda = 0;
  lambda_bin = vector<double>(NUM_BIN, EPS_LAMBDA);
  d_lambda_bin = vector<double>(NUM_BIN, 0.0);

  double best_test = 2147483647.0;
  double scale = 1.0;
  for(int iter = 1; iter <= max_iter ; iter++){
    if(iter % 10 == 0){
      scale *= 0.95;
    }
    cerr <<"Iter: "<<iter+1<<" ------loglik(train_data) = "<<evalLoglik(train_data)<<endl;
    double test_log_lik = evalLoglik(test_data);                                                                   
    if(test_log_lik < best_test){                                               
      best_test = test_log_lik;                                                 
    }                                                                           
    cerr <<"Iter: "<<iter+1<<" ------loglik(test_data)  = "<<test_log_lik<<endl;
    cerr <<"Iter: "<<iter+1<<" ------best test loglik   = "<<best_test<<endl;  
    for(int i = 0 ; i < (int)train_data.size() ; i++){
      if(i % 200000 == 0){
        cout <<"Training with SGD: " << i<<"/"<<train_data.size()<<endl;
      }
      DataPoint & _point = train_data[i];
      long uid = _point.uid;                                                         
      double y = _point.y;
      int bin = min(NUM_BIN-1,(int)(y/(double)BIN_WIDTH));
      double divider = 1.0/(lambda_bin[bin] + lambda_u[uid] + lambda);
      d_lambda = momentum * d_lambda - lr_lambda  * scale * (y - divider);
      d_lambda_u[uid] = momentum * d_lambda_u[uid] - lr_lambda_u * scale * (y - divider);
      for(int b = 0 ; b < bin ; b++){
        d_lambda_bin[b] = momentum * d_lambda_bin[b] - lr_lambda * scale * BIN_WIDTH;
        lambda_bin[b] += d_lambda_bin[b];
        lambda_bin[b] = max(lambda_bin[b], EPS_LAMBDA);
      }
      d_lambda_bin[bin] = momentum * d_lambda_bin[bin] 
        - lr_lambda * scale *  ((y - bin*BIN_WIDTH) - divider);
      

      lambda_bin[bin] += d_lambda_bin[bin];
      lambda_bin[bin] = max(lambda_bin[bin], EPS_LAMBDA); 
      lambda += d_lambda;
      lambda_u[uid] += d_lambda_u[uid];
      lambda = max(lambda, EPS_LAMBDA);
      lambda_u[uid] = max(lambda_u[uid], EPS_LAMBDA);
    }
  }

  cerr <<"finished training "<< string(modelName());                            
  // evalTrainPerp(data);
  return 0;
}
ModelBase::PredictRes PiecewiseConstantModel::predict(const User &user){
  return PredictRes(-1, 0.0, 0.0, false);
  auto ite = _user_train->find(user.id());
  if(ite == _user_train->end() || ite->second.get_sessions().size() == 0){
    return PredictRes(-1, 0.0, 0.0, false);
  }else{
    // this compute 1/n_session * p(t' <= t), not the log-likelihood...
    const vector<Session> &train_sessions = ite->second.get_sessions();
    const vector<Session> &test_sessions = user.get_sessions();
    double loglik = 0.0;
    double prev_end = train_sessions.back().end.hours();
    double lambda = lambda_u[user.id()];
    int num_sessions = (int)test_sessions.size();
    for(int i = 0 ; i < num_sessions ; i++){
      double log_density = log(lambda);
      double integral_lambda = lambda*(test_sessions[i].start.hours() - prev_end);
      // 1 - G(t) = 1 - exp(-int_{0}^t lambda(t) dt)
      loglik += log_density - integral_lambda;
      prev_end = test_sessions[i].end.hours();
    }   

    return PredictRes(0,
        loglik,
        num_sessions,
        true);
  } 
}
const char * PiecewiseConstantModel::modelName(){
  return "piecewise_constant_model";    
}
