#include "ballot.hpp"

void ballot::setowner(name username, uint8_t count){
  require_auth(username);
  uint64_t owner_count;
  for (auto itr = _owner.begin(); itr != _owner.end(); ++itr) {
      ++count;
  }
  eosio_assert(owner_count>0,"Owner is already set.");
  auto itr = _owner.begin();
  itr = _owner.emplace(username,[&](auto& new_owner){
    new_owner.owner = username;
    new_owner.contestants_count = count;
  });

  auto voter_iterator = _voters.find(username.value);
  if(voter_iterator == _voters.end()){
    voter_iterator = _voters.emplace( username, [&](auto& new_voter){
      new_voter.username = username;
      new_voter.weight = 1;
    });
  }
}

void ballot::giveright(name username, name to){
  require_auth(username);
  auto& own = _owner.get(username.value,"You are not owner");
  eosio_assert(username == own.owner,"Only owner hace right to give others to vote.");
  auto voter_iterator = _voters.find(username.value);
  if(voter_iterator == _voters.end()){
    voter_iterator = _voters.emplace( username, [&](auto& new_voter){
      new_voter.username = username;
      new_voter.weight = 1;
    });
  }else{
    auto& votee = _voters.get(to.value,"You are not yet registered.");
    eosio_assert(votee.voted==false,"You have already voted.");
    _voters.modify(votee, to, [&](auto& modified_voter){
      modified_voter.weight = 1;
    });
  }
}

void ballot::delegate(name username, name to){
  require_auth(username);
  name voted_for;
  auto& votee = _voters.get(username.value,"You are not yet registered.");
  eosio_assert(votee.voted==false,"You have already voted.");
  _voters.modify(votee, to, [&](auto& modified_voter){
    modified_voter.delegate = to;
  });
  auto& dvotee = _voters.get(to.value,"The person you want to delegate your vote is not registered.");
  _voters.modify(dvotee, to, [&](auto& modified_voter){
    if(dvotee.voted==false){
      modified_voter.weight += 1;
    }else{
      voted_for = dvotee.votedfor;
      auto& contestee = _contestants.get(voted_for.value,"This person is not the contestant.");
      _contestants.modify(contestee, voted_for, [&](auto& modified_contestor){
        modified_contestor.votecount += 1;
      });
    }
  });
}

void ballot::vote(name username, name to){
  require_auth(username);
  auto& votee = _voters.get(username.value,"You are not yet registered.");
  eosio_assert(votee.voted==false,"You have already voted.");
  _voters.modify(votee, username, [&](auto& modified_voter){
    modified_voter.votedfor = to;
    modified_voter.voted = true;
  });
  auto& contestee = _contestants.get(to.value,"This person is not the contestant.");
  _contestants.modify(contestee, to, [&](auto& modified_contestor){
    modified_contestor.votecount += votee.weight;
  });
}

name ballot::winner(){
  uint8_t votes = 0;
  name winner;
  for(auto& item: _contestants){
    if(item.votecount>votes){
      votes = item.votecount;
      winner = item.username;
    }
  }
  return winner;
}

EOSIO_DISPATCH(ballot,(setowner)(giveright)(delegate)(vote))
