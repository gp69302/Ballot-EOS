#include<eosiolib/eosio.hpp>//gives access to eosio libraries

using namespace std;
using namespace eosio;

class [[eosio::ballot]] ballot: public eosio::contract{
  private:
    struct [[eosio::table]] voter_info{
      name username; // to store voter account name
      uint8_t weight = 0; // weight is vote power. Generally every one's vote power will be one. If someone delegates their vote to this voter, then this voter weight increases.
      name votedfor; // The contestant whom he voted.
      bool voted = false; // It says whether he is voted or not.
      name delegate; // delegate - the voter account name whom we want to give power to vote on behalf of us.

      auto primary_key() const{
        return username.value; // It is used to store voter details in table and to access the stored data with the help of username.
      }
    };

    struct [[eosio::table]] contestant_info{
      name username; // To store the account number of contestant
      uint8_t votecount = 0; // To store the details of votes the contestant got

      auto primary_key() const{
        return username.value;
      }
    };

    struct [[eosio::table]] owner_info{
      name owner;
      uint8_t contestants_count = 0;

      auto primary_key() const{
        return owner.value;
      }
    };
    typedef eosio::multi_index<name("voters"),voter_info>voters_table;
    typedef eosio::multi_index<name("contestants"),contestant_info>contestants_table;
    typedef eosio::multi_index<name("owner"),owner_info>owner_box;

    voters_table _voters;
    contestants_table _contestants;
    owner_box _owner;

  public:
    ballot(name receiver, name code, datastream<const char*> ds):contract(receiver, code, ds),
                  _voters(receiver, receiver.value),
                  _contestants(receiver, receiver.value),
                  _owner(receiver, receiver.value){}

    [[eosio::action]]
    void setowner(name username,uint8_t count);

    [[eosio::action]]
    void giveright(name username, name to);

    [[eosio::action]]
    void delegate(name username, name to);

    [[eosio::action]]
    void vote(name username, name to);

    [[eosio::action]]
    name winner();

};
