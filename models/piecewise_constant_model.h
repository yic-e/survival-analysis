#ifndef __PIECEWISE_CONSTANT_MODEL_H__
#define __PIECEWISE_CONSTANT_MODEL_H__
#include "data_io.h"
#include "model_base.h"
#include <iostream>

class PiecewiseConstantModel : public ModelBase{
  public:
    int train(const UserContainer *data);
    const char *modelName();
    PredictRes predict(const User &user);
    typedef std::unordered_map<long, std::vector<double>> LambdaU;
  private:
    const UserContainer *_user_train;
    LambdaU lambda_u;
};
#endif