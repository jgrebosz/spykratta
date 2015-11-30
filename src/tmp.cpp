//_____________________________________________________________________________
// Getting the events  from the file_________________________________
const_daq_word_t*  TGo4MbsFileParameter::give_next_event(int *how_many_words)     // virtual
{
    cout << "F. TGo4MbsFileParameter::give_next_event " << endl;

#ifdef OLD_PISOLO_DATA
    give_next_old_pisolo_event(how_many_words);
#else

    //
    //      cout << "GGGG nr_of_file_already_analysed = " <<   nr_of_file_already_analysed
    //      << "sizeof(long) = " << sizeof(long)
    //      << endl;
#ifdef NIGDY
    /*     Hi Jurek,
    I send you a compressed file which contains about 50MB of raw data taken
    using 2 ADC Caen V785 and 1 Scaler. I have attached the manual of the
    V785 ADC, that may be useful to understand the format of the data coming
    from the ADCs (see page 41). The Scaler can be ignored.
    The file zipped in data4jurek01.tgz has this format:

    File Header 32 bytes
    first data Block:
    first ADC Block (see output buffer section in Caen V785 Manual,
    page 41):
    Header 32 bits - first is 1a002000
    16 Data Words 32 bits
    End Of Block 32 bits - first is 1c000000
    second ADC Block (as before)
    Header 32 bits - first is 22002000
    16 Data Words 32 bits each
    End Of Block 32 bits - first is 2c7c0000
    Scaler Block
    ignore this...
    Trailer 32 bits, always ffffffff
    second data Block:
    ...
    ...
    File Trailer 24 bytes

    You can see the content of the file using a tool like octaldump (od),
    and the values I wrote are read with the command
    od -tx4 data_pc1203_r1.i0 | more

    I Hope you can use this data easily. Please write me some comments soon...
    Thanks a lot!   ciao   pietro

    ps. we have used V785, not V785N !
#######################################################################
    */
#endif // NIGDY

    cout << "Getting the next event from the FILE and returning the pointer to the data"
         << " nr_of_file_already_analysed = " << nr_of_file_already_analysed
         << endl;

    // reading something from the file into the buffer
top:

    // skipping file header 40/32 bytes was done while opening the file

    //  const_daq_word_t *ptr = ( const_daq_word_t * ) &data[0];
    //   int skok = sizeof (const_daq_word_t);

    long *ptr = (long *) &data[0];
    int skok = sizeof(long);

    cout << "sss buffor to read in is 0x" << hex << reinterpret_cast<long>(ptr) << dec << endl;

    // loop to read words belonging to one event
    int byte_nr = 0 ;
    for(event_length = 0 ; ; event_length += skok, ptr++ , byte_nr++)
    {

        cout << "skok = " << skok << endl;
        file.read((char*) ptr, skok);

        if(file.eof())   // || block_number > when_to_stop)
        {
            cout << "1 End of event file  while reading the event  nr_of_current_event =  " << nr_of_current_event
                 << endl;
//             cout << "1 Read byte nr " << gdzie
//             << " what is " << (100.0 *gdzie)/file_size  << "%  of " << file_size<< endl;
            if(open_next_file_from_the_list())
            {
                goto top;
            }
            else
            {
                cout << "TGo4MbsFileParameter::give_next_event   --->  Error while reading event !!!!!!" << endl;
                exit(3);
            }
        }

        // if this is another error than EOF
        else if(!file)
        {
            cout << "TGo4MbsFileParameter::give_next_event --->  Error while reading event !" << endl;
            exit(3);
        }


        // is it an end of the event?
        if(*ptr == 0xffffffff /*|| event_length > 1000*/)
        {
            nr_of_current_event++;
            //if ( nr_of_current_event > 236000 )
            {
                //                         cout << "End of event nr " << nr_of_current_event
                //                         << ", after " << event_length << " bytes" << endl;
            }

            *how_many_words = event_length / skok;
            break;
        }
        else
        {
            //     if ( nr_of_current_event > 236000 )
            //                   {
            //                       cout  << event_length/skok << ").     read longword   0x"
            //                       << hex << *ptr
            //                       << " --->ev nr   0x" << event_length << dec << endl;
            //                   }
        }
    } // end of for
    // setting some values (for example length of valid data)
#endif

    // returning the pointer to the memory where the event is now
    return (const_daq_word_t*) data;
}


//_____________________________________________________________________________
// Getting the events  from the file_________________________________
const_daq_word_t*  TGo4MbsFileParameter::give_next_old_pisolo_event(int *how_many_words)     // virtual
{
    //   cout << "F. TGo4MbsFileParameter::give_next_old_pisolo_event " << endl;
    //
    //      cout << "GGGG nr_of_file_already_analysed = " <<   nr_of_file_already_analysed
    //      << "sizeof(long) = " << sizeof(long)
    //      << endl;
#ifdef NIGDY
    /*
      The data format of the pisolo file taken with the old daq system is the following:
    First of all, data are organized in 16 bit words.
    Each file start with a 32768 bytes of header that can be ignored.
    Each event start with an header word: 0xff00
    The next word is a counter for the event lenght (it includes event header also).
    Then you've three words of timestamp that can be ignored.
    The next twelve words following the timestamp are the adc samples.
    Each adc has four input channels, this means that the first four words
    are the channels coming from the first adc, the next four the channels
    of the second and the last four the channels of the third adc.

    You can find some data files in the pc1297 (pisolo user), path:
    /data/pisolo/40Ca+40-48Ca_apr10_fromAlpha
    That's all about the data format.
      */
#endif // NIGDY

    //   cout << "Getting the next old pisolo event from the FILE and returning the pointer to the data"
    //   << endl;

    // reading something from the file into the buffer
top:

    // skipping file header 40/32 bytes was done while opening the file

    typedef   unsigned short int   oldpisolo_t;
    oldpisolo_t  *ptr = (oldpisolo_t  *) &data[0];
    int skok = sizeof(oldpisolo_t);

    // loop to read words belonging to one event
    int word_nr = 0 ;
    int given_length_of_event = 999999 ;

    for(event_length = 0 ;     word_nr < given_length_of_event  ;  ptr++ , word_nr++)
    {
        file.read((char*) ptr, skok);
        //###############
        if(file.eof())   // || block_number > when_to_stop)
        {
            cout << "1 End of event file  while reading the event  nr_of_current_event =  " << nr_of_current_event
                 << endl;
            //       cout << "1 Read byte nr " << gdzie
            //       << " what is " << (100.0 *gdzie)/file_size  << "%  of " << file_size<< endl;
            if(open_next_file_from_the_list())
            {
                goto top;
            }
            else
            {
                cout << "TGo4MbsFileParameter::give_next_event   --->  Error while reading event !!!!!!" << endl;
                exit(3);
            }
        }

        // if this is another error than EOF
        else if(!file)
        {
            cout << "TGo4MbsFileParameter::give_next_event --->  Error while reading event !" << endl;
            exit(3);
        }

        //#################
        // analysing what was read
        //     cout << word_nr << " ) " << hex << *ptr << dec << endl;
        if(word_nr == 0)
        {
            if(*ptr == 0xff00)
            {
                //       cout << "Start of reading in the event " << endl;
                continue;
            }
            else
            {
                //       cout << "Lost the synchronisation while reading in the event " << endl;
                goto top;
            }

        }

        if(word_nr == 1)
        {
            given_length_of_event = *ptr;
            //       cout << "This event will have " << given_length_of_event << " words" << endl;
            continue;
        }

        // is it an end of the event?
        if(word_nr > 1000)
        {
            cout << "Such a long event ?   word_nr ="
                 << word_nr << ", given_length_of_event = " << given_length_of_event << endl;
            given_length_of_event = 0;
            goto top;
            break;
        }
        else
        {
            // normal situation
            //     if ( nr_of_current_event > 236000 )
            //                   {
            //                       cout  << event_length/skok << ").     read longword   0x"
            //                       << hex << *ptr
            //                       << " --->ev nr   0x" << event_length << dec << endl;
            //                   }
        }

        if(*ptr == 0xff00 && (word_nr != 0))
        {
            cout << "Beginning of the next event, should never happen " << endl;
            goto top;  // this would be the next event
        }

    } // end of for
    // setting some values (for example length of valid data)

    nr_of_current_event++;
    //if ( nr_of_current_event > 236000 )
    {
        //     cout << "End of event nr " << nr_of_current_event
        //     << ", after " << word_nr << " words" << endl;
    }

    *how_many_words = given_length_of_event;
    // returning the pointer to the memory where the event is now
    return (const_daq_word_t*) data;
}
//***************************************************************************



//***************************************************************************
/*!
    \fn TGo4MbsFileParameter::open_file(string name)
 */
bool TGo4MbsFileParameter::open_file(string filename)
{

    cout << "TGo4MbsFileParameter::open_file  [" << filename << "]" << endl;

    if(filename[0] == '@')    // if this is a list of files to analyse
    {
        flag_list = true;
        nr_of_file_already_analysed = 0 ;
        cout << "This is a LIST of files. " << endl;
        string name_without_at = filename.substr(1);
        ifstream plik(name_without_at.c_str());
        if(!plik)
        {
            cout << "Can't open the file " << name_without_at << " with the list of files  --> called " << endl;
            return false;
        }

        // Readint the contents of the file with the list and creating a vector with these names.
        string single_name;
        for(int i = 0 ; plik.good() ; i++)
        {
            plik >> single_name ;
            cout << (i + 1) << ".  file on the list is: " << single_name << endl;

            if(!plik) break;
            list_of_names.push_back(single_name);
        }
        filename = list_of_names[0];
    }
    else  // if this is a name of the single file to analyse
    {
        flag_list = false;
        list_of_names.push_back(filename);  // put this name on the list
    }

    //  plik.close();
    file.open(filename.c_str(), ios::binary);
    if(!file)
    {
        cerr << "Can't open file " << filename << endl;
        return false;
    }

    cout << "\nTGo4MbsFileParameter::open_file(string name)      File   " << filename  << "    successfully open for analysis " << endl;
    current_file = filename;




    file.seekg(0, ios::end);
    file_size = file.tellg();
    file.seekg(0, ios::beg);
    long pocz = file.tellg();
    file_size -= pocz;


    cout << " TGo4MbsFileParameter::open_file    ===>   nr_of_file_already_analysed = " << nr_of_file_already_analysed << endl ;
    cout << "1 list_of_names.size() = " << list_of_names.size() << endl;
    cout << "1 buffor to read in is 0x" << hex << (long)data << dec << endl;
    return true;
}
//***************************************************************
bool TGo4MbsFileParameter::open_next_file_from_the_list()
{

    cout << "TGo4MbsFileParameter::open_next_file_from_the_list() " << endl;

    if(!flag_list)
    {
        cout << "The whole list file is analysed" << endl;
        Texception_input_file x(myEOF);
        throw x;
    }
    nr_of_file_already_analysed++;
    if(nr_of_file_already_analysed >= list_of_names.size())
    {
        cout << "222 All files are already analysed" << endl;
        Texception_input_file x(myEOF);
        throw x;
    }
    file.close();
    cout << "222 File nr " << nr_of_file_already_analysed << endl;
    // repair the error state

    // repair the stream
    file.clear(file.rdstate() & ~(ios::eofbit | ios::failbit));

    cout << "222 list_of_names.size()" << list_of_names.size() << endl;
    if(nr_of_file_already_analysed >= list_of_names.size())
    {
        cout << "ALArm - bigger than the vector?" << endl;
    }

    cout
            << "222 nr_of_file_already_analysed = "  << nr_of_file_already_analysed
            << endl;


    cout
            << "222 Current file to analysis  has a name "
            << list_of_names.at(nr_of_file_already_analysed)
            << endl;

    // opening the data file to analyse
    file.open(list_of_names[nr_of_file_already_analysed].c_str(),  ios::binary);
    if(!file)
    {
        cerr << "Can't open file with events " << list_of_names[nr_of_file_already_analysed] << endl;
        Texception_input_file x(myFAIL);
        throw x;
    }
    cout << "File " << list_of_names[nr_of_file_already_analysed]
         << " successfully open for analysis " << endl;

    file.seekg(0, ios::end);
    file_size = file.tellg();
    file.seekg(0, ios::beg);
    long pocz = file.tellg();
    file_size -= pocz;

    current_file = list_of_names[nr_of_file_already_analysed];
    // read_in_next_block();  // skiping info block? - not needed for Exotic
    //    read_in_next_block();    // reading the real data block <---- no in case of exotic, the proper function will ask for event in proper time,

    // skipping file header 40 bytes
    file.read(data, 10 * sizeof(long));
    if(!file)
    {
        cout << "2. Error while reading (skipping) file header of the data 40 dwords" << endl;
    }
    // if successful,
    return true;
}
//*************************************************************************
