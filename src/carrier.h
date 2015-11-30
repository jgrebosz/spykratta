#ifndef  _CARRIER_H_
#define  _CARRIER_H_

#ifdef NIGDY

///////////////////////////////////////////////////////////////////////////////
class transporter : public TGo4Parameter
{
private:
    int value_of_sth ;
public:

    //-----------------------
    transporter(string * name, string * title) : TGo4Parameter(name, title)
    {
        value_of_sth = 10 ;
    }

//----------------------------
    int PrintParameter(string * n = 0, int buflen = 0)
    {
        cout << "my Function transporter::PrintParameter with arguments,text n="
             << n
             << " buflen int = "
             << buflen
             << endl ;

        return value_of_sth ;
    }
//------------------------------
    int Print(Option_t * dummy)
    {
        cout << "Function transporter::Print with argument,dummy ="
             << dummy
             << endl ;

        return value_of_sth ;
    }

//-----------------------------
    Bool_t UpdateFrom(TGo4Parameter* rhs)
    {

        cout << "We are in F. transporter::UpdateFrom  " << endl;
        cout << "value of pointer rhs = " << rhs << endl ;

        if(rhs) return kFALSE;

        cout << "after tested return " << endl;


        // this method should better be pure virtual.
        // however, we have to implement dummy for root
        // to let it clone and stream this with baseclass type
        cout << "GO4> !!! ERROR: TGo4Parameter::UpdateFrom() not implemented!!!"
             << endl;
        cout << "GO4> Please overwrite virtual method in your class: ";
        cout << IsA()->Class()->GetName() << endl;
        return kFALSE;
    }
//----------------------------
    void ustaw(int liczba)
    {
        //cout << "wstawienie nowej liczby " << liczba <<endl ;
        value_of_sth = liczba;
    }

};

#endif // NIGDY


#endif  // _CARRIER_H_
