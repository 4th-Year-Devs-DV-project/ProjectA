/**

* Ben Earle and Kyle Bjornson

* ARSLab - Carleton University

*

* Blinky:

* Simple modle to toggle the LED using DEVS internal transitions.

*/



#ifndef BOOST_SIMULATION_PDEVS_receiver_HPP

#define BOOST_SIMULATION_PDEVS_receiver_HPP



#include <stdio.h>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>
#include <math.h> 
#include <assert.h>
#include <memory>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <limits>
#include <random>

#include "../drivers/nRF24L01P.h"

#define TRANSFER_SIZE   4



using namespace cadmium;

using namespace std;



//Port definition

    struct receiver_defs {

        struct dataOut : public out_port<bool> { };

        struct in : public in_port<bool> { };

        //struct idIn : public in_port<float> { };

    };



    template<typename TIME>

    



  class receiver{

    

  using defs=receiver_defs; // putting definitions in context

  public:

  TIME slowToggleTime;



  receiver(PinName s, PinName t, PinName q, PinName w, PinName r, PinName a){
    slowToggleTime = TIME("00:00:20:00");
    state.temp = new nRF24L01P(s,t,q,w,r,a);
    state.newTag = false;
  }

    

  // state definition

  struct state_type

  {

    char *s;

    nRF24L01P* temp;

    bool newTag;

  }; 

  state_type state;



  using input_ports=std::tuple<>;

  using output_ports=std::tuple<typename defs::dataOut>;



  // internal transition

  void internal_transition() {

	printf( "nRF24L01+ Frequency    : %d MHz\r\n",  state.temp->getRfFrequency() );
        printf( "nRF24L01+ Output power : %d dBm\r\n",  state.temp->getRfOutputPower() );
        printf( "nRF24L01+ Data Rate    : %d kbps\r\n", state.temp->getAirDataRate() );
        printf( "nRF24L01+ RX Address   : 0x%010llX\r\n", state.temp->getRxAddress() );

	

	state.temp->powerUp(); // power up antenna 
	state.temp->setReceiveMode();
	state.temp->enable(); // enable to ce pin
	
	
	state.temp->read(NRF24L01P_PIPE_P0, state.s, 8); // getting hello world
	printf("%i \n",sizeof(state.s));
	printf( "reciving data:%s \n",state.s);
	
	

  }





  // external transition

  void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {



  }

  // confluence transition

  void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {

    internal_transition();

    //external_transition(TIME(), std::move(mbs));

    printf("here");

  }



  // output function

  typename make_message_bags<output_ports>::type output() const {

    typename make_message_bags<output_ports>::type bags;

    bool out;              

    out = (state.newTag ? 1: 0);

    //out = false;

    get_messages<typename defs::dataOut>(bags).push_back(out);                    

    return bags;

  }



  // time_advance function

  TIME time_advance() const {  

      return slowToggleTime;

  }



  friend std::ostringstream& operator<<(std::ostringstream& os, const typename receiver<TIME>::state_type& i) {

    os << "Output: " << (i.newTag ? 1 : 0); 

     //os << "Output: " << false; 

    return os;

  }

};     

#endif // BLINKY_HPP
