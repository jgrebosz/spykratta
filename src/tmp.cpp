#define PARIS_OTERWISE_PLASTIC false
#if PARIS_OTERWISE_PLASTIC



            auto current_board = ltb_paris.FindBoard(visit_ptr->name);
            // what if the Find was not succesful?

            double time_in_channel_0 = 0;
            for(int n = 0 ; n < d.GetNchan() ; ++n)
            {
                if(d.Empty(n))
                    continue;

                int id_parysa = current_board[n]->GetID();

                for(int s = 0 ; s < 3; ++s) // we need 3 first data signals (time, short, long)
                {
                    auto wart = d.GetData(n, s);
                    if(wart != 0){
                        // distributing to the unpacked event


                        if(s == 0) // Note in case of time signal it should be
                            //subtracted by the contents of channel 0
                        {
                            if(n == 0) { time_in_channel_0 = wart;}
                            wart -= time_in_channel_0;
                        }

                        double multiplier = (s != 0)? 0.125 :  100.0;
                        double offset = (s != 0)? 0 :  1000.0;


                        int where = id_parysa + (s * 32);    // which signal:
                        // because 0-31 is time, 32-63 qshort (fast), 64-96 qlong(slow)

                        // Paris takes directly from this array
                        target_event->digitizer_data[where] = wart * multiplier + offset ;       // int32 array, (for BaF)
                        target_event->digitizer_int16data[where] = wart * multiplier + offset ;  // short int array (for Tone_signal)

                        //                                              cout << "Channel n =" <<  n
                        //                                                   << "  (paris_" << id_parysa
                        //                                                  << "), signal " << s << " ==> value "
                        //                                                      << wart << "  po korekcji = "
                        //                                                      <<  wart * multiplier +offset << endl;



                    }
                }
            }


