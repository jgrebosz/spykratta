#include "Tuser_incrementer.h"
#include "Tfile_helper.h"

#ifdef USER_INCREMENTERS_ENABLED
#include <fstream>
#include <stdexcept>
// static
vector<Tuser_incrementer*>  * Tuser_incrementer::user_incrementers_ptr;
int Tuser_incrementer::limit_of_nesting = 1000;
//**************************************************************************************************************
Tuser_incrementer::Tuser_incrementer(
    string name_of_result,
    string name_of_component_a,
    operation_type oper,
    string name_of_component_b)
    :
    name_result(name_of_result),
    name_a(name_of_component_a),
    name_b(name_of_component_b),
    operation(oper)
{
    cout << "Is it really never used?" << endl;
    exit(4);
//     set_parameters(name_of_result,
//                    name_of_component_a,
//                    oper,
//                    name_of_component_b,
//                    false,  // fake
//                    1);   // fake
}
//*****************************************************************************************
void  Tuser_incrementer::set_parameters(string name_of_result,
                                        string name_of_component_a,
                                        bool  a_incr_not_constant,
                                        double conA,
                                        operation_type   oper,
                                        string name_of_component_b,
                                        bool  b_incr_not_constant,
                                        double conB
                                       )
{

    a_is_incrementer_not_constant = a_incr_not_constant;
    b_is_incrementer_not_constant = b_incr_not_constant;

	 Tincrementer_donnor *donnor = nullptr;
	 
    // at first we try to get information about the components of this arithmetics
    Tmap_of_named_pointers::iterator   pos;
    /*
    *
    *   pos = named_pointer.find(name_of_component_a);
       if(pos == named_pointer.end())
       {
    //         a_ptr = NULL;   // <--- wrong, it does not exist
    //         if(!a_ptr)
           cout << "Tuser_incrementer::ctor  -  \n"
                " in user incrementr called " << name_of_result <<
                "\n   the component: 'green'  incrementer: "
                << name_of_component_a << " was not found " << endl;
           // perhap this is an user incrementer name
           // if yes, read it at first
           create_one_user_incrementer(entry_name );   // <<<<----------------FIXME
           pos = named_pointer.find(name_of_component_a);
           if(pos == named_pointer.end()) {
               cerr << "After second try - impossible " << endl;
               exit(32);
           }

       }

       a_ptr = &(named_pointer[name_of_component_a]);
    */
    // GREEN operand ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if(a_is_incrementer_not_constant)
    {
        //Tmap_of_named_pointers::iterator
        string entry_name =  name_of_component_a ;
        pos = named_pointer.find(name_of_component_a);
        if(pos == named_pointer.end())
        {

            cout  << "Tuser_incrementer::ctor  -  \n"
                  " in user incrementr called " << name_of_result
                  << " the component: 'Green' incrementer: "
                  << name_of_component_a << " was not found " << endl;
            create_one_user_incrementer(entry_name );   // <<<<----------------FIXME
            pos = named_pointer.find(name_of_component_a);
            if(pos == named_pointer.end()) {
                cerr << "After second try - impossible " << endl;
                exit(34);
            }

        }

        a_ptr = &(named_pointer[name_of_component_a]);
		  donnor = a_ptr->ptr_detector;
    } else {  // the second operand is constant
        a_ptr = nullptr;
        constantA = conA;
    }


// BLUE operand ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if(b_is_incrementer_not_constant)
    {
        //Tmap_of_named_pointers::iterator
        string entry_name =  name_of_component_b ;
        pos = named_pointer.find(name_of_component_b);
        if(pos == named_pointer.end())
        {

            cout  << "Tuser_incrementer::ctor  -  \n"
                  " in user incrementr called " << name_of_result
                  << " the component: 'Blue' incrementer: "
                  << name_of_component_b << " was not found " << endl;
            create_one_user_incrementer(entry_name );   // <<<<----------------FIXME
            pos = named_pointer.find(name_of_component_a);
            if(pos == named_pointer.end()) {
                cerr << "After second try - impossible " << endl;
                exit(34);
            }

        }

        b_ptr = &(named_pointer[name_of_component_b]);
		  if(donnor != nullptr)
		  {
			 donnor = b_ptr->ptr_detector;
		  }
    } else {  // the second operand is constant
        b_ptr = nullptr;
        constantB = conB;
    }
    //=============================================================================
    // now we can create our incrementer
    // searching if it already exists
    pos = named_pointer.find(name_of_result);
    // if it already exist ?
    if(pos != named_pointer.end())
    {
        // if yes, remove it
        named_pointer.erase(pos);
    }
    // putting once more
    named_pointer[name_of_result]
        = Tnamed_pointer(&result,   &result_validator,
                         nullptr,         // a_ptr->ptr_detector  // instead of   "this"
                         this // <---- THIS IS A    USER DEFINIED INCREMENTER adres - it will be use if
                         // some us.def.inc is using the other. so the other have to be avaluated at first
                        ) ;

}
//******************************************************************************************************************************
//******************************************************************************************************************************
void Tuser_incrementer::evaluate_incrementer_for_current_event()
{
    static int level_of_nesting;
    static vector<string> stack_of_nesting_names;

    if(already_evaluated) return;

    //cout << " Tuser_incrementer::" << __func__  << " for incr called " << name_result   << endl;
    stack_of_nesting_names.push_back(name_result);
    result = 0;

#if 0
    // a_ptr is always obligatory -------------------------------------------------------------
    if(!a_ptr) {
        cout << "Tuser_incrementer: Component incrementer: "
             << name_a << " was not found " << endl;
        exit(32);
    }

    // if this is an user incrementer at first evaluate him
    if(a_ptr->is_user_incrementer() ) // let's evaluate  it at first
    {
        if(++level_of_nesting > limit_of_nesting)
        {
            cerr << "Tuser_incrementer::" << __func__  << " for incremener " << this->name_result
                 << " - level of nesting is > " << limit_of_nesting
                 << " - so this is most probably a logic error (infinite loop)" << endl;

            int how_many_to_show =  stack_of_nesting_names.size();
            //cout << "how_many_to_show =" <<  how_many_to_show << endl;
            for(int i = 0 ; i < min(20, how_many_to_show) ; ++i)
            {
                cerr << stack_of_nesting_names[i] << " --> " ;
            }
            exit(55);

        }

        // cout << "the nested green incrementer " << name_result << " is user definied.  We need to calculate it first " << endl;
        a_ptr->give_user_incr_adr()->evaluate_incrementer_for_current_event();
        --level_of_nesting;
    }

    bool  flag_a = (a_ptr->data_usable) ? *(a_ptr->data_usable) : true;   // 0 (NULL) means: always valid

#endif // 0





    //=== Operand A ================================================================================
    double first_operand = 0 ;
	  bool flag_a = true;
    if(a_is_incrementer_not_constant)   // second is both are incrementes
    {
        if(!a_ptr) {
            cout << "Tuser_incrementer: Component incrementer: " << name_a << " was not found " << endl;
            exit(4);
        }

        // if this is an user incrementer at firs evaluate him
        if(a_ptr->is_user_incrementer() )
        {

            if(++level_of_nesting > limit_of_nesting)
            {
                cerr << "Tuser_incrementer::" << __func__  << " for incremener " << this->name_result
                     << " - level of nesting is > " << limit_of_nesting << " - so this is most probably a logic error (infinite loop)" << endl;
                cerr << "Wrong logic of nesting calls of user incrementers starting from calling " << endl ;

                int how_many_to_show =  stack_of_nesting_names.size();
                for(int i = 0 ; i < min(10, how_many_to_show) ; ++i)
                {
                    cerr << stack_of_nesting_names[i] << " --> " ;
                }
                exit(56);

            }
            // cout << "the nested green incrementer " << name_result << " is user definied.  We need to calculate it first " << endl;
            a_ptr->give_user_incr_adr()->evaluate_incrementer_for_current_event();
            --level_of_nesting;
        }

        flag_a = (a_ptr->data_usable) ?  *(a_ptr->data_usable)  : true;   // 0 (NULL) means: always valid       
        //first_operand =  *(a_ptr->double_ptr);
		  first_operand = give_incrementer_value(a_ptr);

    } else { // first  is constant
        first_operand = constantA;
    }

    //===================================================================================
    double second_operand = 0 ;
    if(b_is_incrementer_not_constant)   // second is both are incrementes
    {
        if(!b_ptr) {
            cout << "Tuser_incrementer: Component incrementer: " << name_b << " was not found " << endl;
            exit(4);
        }

        // if this is an user incrementer at firs evaluate him
        if(b_ptr->is_user_incrementer() )
        {

            if(++level_of_nesting > limit_of_nesting)
            {
                cerr << "Tuser_incrementer::" << __func__  << " for incremener " << this->name_result
                     << " - level of nesting is > " << limit_of_nesting << " - so this is most probably a logic error (infinite loop)" << endl;
                cerr << "Wrong logic of nesting calls of user incrementers starting from calling " << endl ;

                int how_many_to_show =  stack_of_nesting_names.size();
                for(int i = 0 ; i < min(10, how_many_to_show) ; ++i)
                {
                    cerr << stack_of_nesting_names[i] << " --> " ;
                }
                exit(56);

            }
            // cout << "the nested green incrementer " << name_result << " is user definied.  We need to calculate it first " << endl;
            b_ptr->give_user_incr_adr()->evaluate_incrementer_for_current_event();
            --level_of_nesting;
        }

        bool flag_b = (b_ptr->data_usable) ?  *(b_ptr->data_usable)  : true;   // 0 (NULL) means: always valid
        result_validator = flag_a &&  flag_b;        
		  second_operand = give_incrementer_value(b_ptr);

    } else { // second is constant
        result_validator = flag_a;
        second_operand = constantB;
    }


    if(first_operand > 10)
    {
        //cout << " first_operand of "   << name_result  << "  = " <<  first_operand << endl;
        result_validator = true ; // FIXME: fake value for testing purposes
    }

    if(result_validator)
    {
        switch(operation)
        {
        case plus:
            result = first_operand + second_operand;
            break;
        case minus:
            result = first_operand - second_operand;
            //         cout << "NEW  tof_21_41_LL_cal_minus_tof_22_41_LL_cal= "
            //         << result
            //         <<", (" <<  *(a_ptr->double_ptr)
            //         << " - " <<  *(b_ptr->double_ptr)
            //         << endl;
            break;
        case multiply:
            result = first_operand * second_operand;
            break;
        case divide:
            if(second_operand)   // not division by zero
            {
                result = first_operand /  second_operand;
            }
            else result_validator = false;
            break;

        case mean:
            result = (first_operand + second_operand) /2.0;
            break;


        }
  
//   if(result > 10)
//  			  cout << "Calculating the incremeter: "
//                << name_result << " = "
//                << result
//               << "  validator = " << result_validator
//               << endl;

     
    }
    
 
    stack_of_nesting_names.pop_back();
    already_evaluated = true;
}
//******************************************************************************************************************************
void Tuser_incrementer::read_in_parameters(string name)
{
    string pat_name = "./incrementers_user_def/" + name ;
    if(pat_name.find(".incr") == string::npos)
    {
        pat_name += ".incr" ;
    }
    ifstream plik(pat_name.c_str());
    if(!plik)
    {
        // cant open file warning
        cout << "Error while opening file: " << pat_name << endl;
        exit(1);
    }
    int op_code = 0 ;

    try {
        FH::spot_in_file(plik, "$user_incrementer_name");
        plik >>  name_result;

        a_is_incrementer_not_constant = ! FH::find_in_file(plik, "$flag_exprA_is_constant");

        FH::spot_in_file(plik, "$other_incrementer_nameA");
        plik >> name_a ;

        constantA = FH::find_in_file(plik, "$constant_valueA");

        op_code  = FH::find_in_file(plik, "$operation");

        constantB = FH::find_in_file(plik, "$constant_valueB");

        b_is_incrementer_not_constant = ! FH::find_in_file(plik, "$flag_exprB_is_constant");

        FH::spot_in_file(plik, "$other_incrementer_nameB");
        plik >> name_b;

    }
    catch(...)
    {
        if(!plik)
        {
            cout << "Error during reading the file " << endl;
            exit(1);
        }
    }

//     FH::spot_in_file(plik, " );
//
//     plik >> name_result ;
//     plik >> name_a ;
//     int op_code = 0 ;
//     plik >> op_code ;
//
    switch(op_code)
    {
    case 1 :
        operation = plus;
        break;
    case 2 :
        operation = minus;
        break;
    case 3 :
        operation = multiply;
        break;
    case 4 :
        operation = divide;
        break;
    case 5:
        operation = mean;
    }

    // nr what: incrementer of constant

    set_parameters(name_result,
                   name_a,   a_is_incrementer_not_constant, constantA,
                   operation,
                   name_b,  b_is_incrementer_not_constant, constantB);

}
//***************************************************************************************************************************************
double Tuser_incrementer::give_incrementer_value(Tnamed_pointer * ptr)
{
    switch(ptr->what_type)
    {
    case Tnamed_pointer::is_double:
        return *(ptr->double_ptr);
// 					 cout << "incremented with value " << (*(a_ptr.double_ptr)) << endl;
    case   Tnamed_pointer::is_int:
        // cout << "Incremet USER with value " << *(a_ptr.int_ptr) << endl ;
        return (*(ptr->int_ptr));
    case   Tnamed_pointer::is_bool:
        return(*(ptr->bool_ptr));
    default:
        throw runtime_error("nonsense in Tuser_incrementer::give_incrementer_value()" );
    } // switch
}
//############################################################################
// Static function - creator
void Tuser_incrementer::create_all_user_incrementers(vector<string> & vector_of_descriptions,  vector<Tuser_incrementer*> & vector_of_user_incrementers )
{
    user_incrementers_ptr = & vector_of_user_incrementers;
    for(auto name_of_description : vector_of_descriptions)
    {
        if(! create_one_user_incrementer(name_of_description))
        {
            cerr << "failure in " << __func__ << endl ;
            exit(44);
        }
    }
}
//###########################################################################
bool Tuser_incrementer::create_one_user_incrementer(string name_of_description )
{
    cerr << __func__ << " " << name_of_description << endl;

//   static int level_of_nesting ;
//
//   if(++level_of_nesting > limit_of_nesting) {
// 	cerr << "level of nesting " << level_of_nesting
// 	<< " is too big - most probaby there is an infinite chain while definig user incrementers " <<
//   }


    string pathed_name_of_description = "./incrementer_user_def/" + name_of_description;

    cout << "In the string " << name_of_description << " lookin for the extension " << endl;

    string name_without_extension = name_of_description;
    string::size_type  extension_starts = name_of_description.find(".incr");
    if(extension_starts != string::npos)
    {
        name_without_extension =
            name_of_description.erase(extension_starts, 100);
    }

    // if it was already created before - return
    for(auto x : *user_incrementers_ptr)
    {
        if(x->name_result == name_without_extension) {
            cout << "User incrementer " << x << " is already on the list" << endl;
            return true;   // this incrementer is already created
        }
    }

    // Check if it is not the snake situation

    static int level_of_nesting;
    static vector<string> nesting_trace;
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    for(unsigned int i = 0; i < nesting_trace.size(); i++) // is this name already on the list?
    {
        if(nesting_trace[i] == name_without_extension)
        {
            cout << "\nUser_incrementers   nesting trace is currently : \n";
            for(unsigned int k = 0; k < nesting_trace.size(); k++)
            {
                cout << "(" << k << ") " << nesting_trace[k] << " --> ";
            }
            cout << name_without_extension;

            cout << "\nERROR: Infinite nesting of the incrementers at the level "
                 << level_of_nesting
                 << "\n - the incrementer called " << name_without_extension
                 << " was already on the level " << i
                 << "\n This \"Snake is eating its own tail\" " << endl;
            exit(1);
        }
    }

    if(++level_of_nesting > limit_of_nesting)
    {
        cout << "Nesting the incrementers reach the level " << level_of_nesting
             << "\n - it is unrealistic, most probably there is a situation "
             "\n 'Snake is eating its own tail' " << endl;
        exit(1);
    }

    nesting_trace.push_back(name_without_extension);

    //  incrementers are not registered in root, they do not have to have
    //  any continuity after any PAUSE, so we can create them new everytime


    Tuser_incrementer *c { new Tuser_incrementer() } ;
    c->read_in_parameters(name_without_extension);     // the path will be added automatically

    // put spectrum on the list
    user_incrementers_ptr->push_back(c);

    level_of_nesting--;
    nesting_trace.pop_back();
    return true;
}
#endif // #ifdef USER_INCREMENTERS_ENABLED
