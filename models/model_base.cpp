#include "model_base.h"
#include "model_test.h"
#include "eval_loglik.h"
#include "global_constant_model.h"
#include "global_piecewise_constant_model.h"
#include "construct_feature_model.h"
#include "user_constant_model.h"
#include "piecewise_constant_model.h"
#include "feature_based_model.h"
#include <string>
jsoncons::json ModelBase::_config;
ModelBase *ModelBase::makeModel(const char *model_name){
    std::string name = model_name;
    if(name == "model_test"){
        return new ModelTest();
    }else if(name == "global_constant_model"){
        return new GlobalConstantModel();
    }else if(name == "user_constant_model"){
        return new UserConstantModel();
    }else if(name == "piecewise_constant_model"){
	    return new PiecewiseConstantModel();
    }else if(name == "feature_based_model"){
        return new FeatureBasedModel();
    }else if(name == "construct_feature_model"){
        return new ConstructFeatureModel();
    }else if(name == "global_piecewise_constant_model"){
        return new GlobalPiecewiseConstantModel();
    }
    return nullptr;
}

EvaluationBase *EvaluationBase::makeEval(const char *eval_name){
    std::string name = eval_name;
    if(name == "eval_loglik"){
        return new EvalLoglik();
    }
    return nullptr;
}
