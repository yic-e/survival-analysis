#include "data_io.h"
#include "global_constant_model.h"
#include <iostream>
using namespace std;

int GlobalConstantModel::train(const UserContainer *data){
    long total_time = 0;
    double session_num = 0; 
    for(auto iter = data->begin();
        iter != data->end(); ++iter){
        int index = 0;
        int prev_time = -1;

        for (auto j = iter->second.get_sessions().begin();
             j!= iter->second.get_sessions().end();
             ++j){
            if (index == 0){
                index++;
                prev_time = j->end;
            } else{
                total_time += (j->start - prev_time);
                prev_time = j->end;
                session_num ++;
            }
        }
    }
    lambda = session_num / total_time;
    return 0;
}

long GlobalConstantModel::predict(long uid){
    return (1/lambda);
}

const char *GlobalConstantModel::modelName(){
    return "global_constant_model";
}	

int main(){
    UserContainer train_data;
    train_data.reserve(10000000);
    read_data("/home/yicheng1/survival-analysis/data/user_survive/daily/show_read_stay.%s",
              "20150703",
              "20150705",
              train_data);
    GlobalConstantModel *model = new GlobalConstantModel();
    model->train(&train_data);
    double lambda = model->lambda;
    cout<<endl<<"lambda: "<<lambda<<endl;
    cout<<"predict-------"<<endl<<"userid: 1234567 return time:"<<model->predict(1234567)<<endl;
 
    return 0;
}