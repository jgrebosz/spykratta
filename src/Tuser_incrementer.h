//
// C++ Interface: Tuser_incrementer
//
// Description:
//
//
// Author: dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TUSER_INCREMENTER_H
#define TUSER_INCREMENTER_H


#include "experiment_def.h"
/**
This is a class which helps the user to define his new incremeters
which are kind of arithmetic on the existing ones.
@author dr. Jerzy Grebosz
*/

#ifdef USER_INCREMENTERS_ENABLED

#include <iostream>
#include <string>
using namespace std;
#include "Tnamed_pointer.h"

class Texception_try_later  { };
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tuser_incrementer
{
protected:
    string name_result;
    string name_a, name_b;
    double constantA;
    bool a_is_incrementer_not_constant {false};
    double constantB;
    bool b_is_incrementer_not_constant {false};


    double result;
    bool result_validator;
    Tnamed_pointer *a_ptr 			{nullptr };
    Tnamed_pointer *b_ptr 			{nullptr };

    bool already_evaluated 			{false};
public:
    enum operation_type { plus = 1, minus, multiply, divide, mean };
protected:
    operation_type operation 			{plus};
	 
    double give_incrementer_value(Tnamed_pointer * ptr);

public:
    Tuser_incrementer(): name_result("NONE"), a_ptr(0), b_ptr(0), operation(plus) {}

    Tuser_incrementer(string name_of_result,
                      string name_of_component_a,
                      operation_type   oper,
                      string name_of_component_b);

    void evaluate_incrementer_for_current_event();
    void read_in_parameters(string name_without_extension);

//     void  set_parameters(string name_of_result,
//                          string name_of_component_a,
//                          operation_type   oper,
//                          string name_of_component_b,
//                          bool  incr_not_constant,
//                          double constant	 );
    void  set_parameters(string name_of_result,
                         string name_of_component_a,
                         bool  a_incr_not_constant,
                         double conA,
                         operation_type   oper,
                         string name_of_component_b,
                         bool  b_incr_not_constant,
                         double conB
                        );


    void reset_new_event() {
        already_evaluated = false;
        result = 0;
    }

    friend
    ostream& operator<< (ostream & s, Tuser_incrementer & u)
    {
        s << u.name_result << " is  using '" ;
        if(u.a_is_incrementer_not_constant)
        {
            s << u.name_a << "' addres= " << u.a_ptr ;
        } else {
            s << " constantA=" << u.constantA  ;
        }

        cout << " and " ;
        if(u.b_is_incrementer_not_constant)
        {
            s << u.name_b << "' addres= " << u.b_ptr ;
        } else {
            s << " constantB=" << u.constantB  ;
        }
        return s;
    }

    static
    void create_all_user_incrementers(vector<string> & name_of_description,  vector<Tuser_incrementer*> & vector_of_user_incrementers );
    static
    bool create_one_user_incrementer(string name);

    static  vector<Tuser_incrementer*>  * user_incrementers_ptr;
    static int limit_of_nesting;
};
#endif // #ifdef USER_INCREMENTERS_ENABLED

#endif
