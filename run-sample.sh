#! /bin/bash
./main --models=global_constant_model \
       --models=user_constant_model \
       --evaluations=eval_test \
       --train_start=20150628 \
       --train_end=20150701 \
       --test_start=20150702 \
       --test_end=20150702 \
       --data_template=data/user_survive/daily/show_read_stay.%s