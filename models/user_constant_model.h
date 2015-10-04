#ifndef __USER_CONSTANT_MODEL_H__
#define __USER_CONSTANT_MODEL_H__
#include "data_io.h"
#include "model_base.h"
#include "user_constant_model.h"
#include <unordered_map>

#include <iostream>

class UserConstantModel : public ModelBase{
  friend class HawkesProcessModel;
  friend class FeatureBasedModel;
  friend class PiecewiseConstantModel;
  public:
    int train(const UserContainer *data);
    const char *modelName();
    PredictRes predict(const User &user);
    double evalTrainPerp(const UserContainer *);
    typedef std::unordered_map<long, double> LambdaU;
  private:
    const UserContainer *_user_train;
    LambdaU lambda_u;
};
#endif
