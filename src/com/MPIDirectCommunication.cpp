// Copyright (C) 2011 Technische Universitaet Muenchen
// This file is part of the preCICE project. For conditions of distribution and
// use, please see the license notice at
// http://www5.in.tum.de/wiki/index.php/PreCICE_License

#ifndef PRECICE_NO_MPI

#include "MPIDirectCommunication.hpp"

#include "utils/Parallel.hpp"
#include "utils/Globals.hpp"

#include <map>

namespace precice {
namespace com {

tarch::logging::Log MPIDirectCommunication::_log(
    "precice::com::MPIDirectCommunication");

MPIDirectCommunication::MPIDirectCommunication()
    : _communicator(utils::Parallel::getGlobalCommunicator())
    , _globalCommunicator(utils::Parallel::getGlobalCommunicator())
    , _localCommunicator(utils::Parallel::getGlobalCommunicator())
    , _isConnected(false) {
}

MPIDirectCommunication::~MPIDirectCommunication() {
  preciceTrace1("~MPIDirectCommunication()", _isConnected);

  closeConnection();
}

size_t MPIDirectCommunication::getRemoteCommunicatorSize() {
  preciceTrace("getRemoteCommunicatorSize()");
  assertion(isConnected());
  int remoteSize = 0;
  MPI_Comm_remote_size(communicator(), &remoteSize);
  return remoteSize;
}

void
MPIDirectCommunication::acceptConnection(std::string const& nameAcceptor,
                                         std::string const& nameRequester,
                                         int acceptorProcessRank,
                                         int acceptorCommunicatorSize) {
  preciceTrace2("acceptConnection()", nameAcceptor, nameRequester);
  assertion(not isConnected());

  utils::Parallel::splitCommunicator(nameAcceptor);

  preciceCheck(utils::Parallel::getCommunicatorSize() > 1,
               "acceptConnection()",
               "ERROR: MPI communication direct (i.e. single) can be only "
                   << "used with more than one process in base communicator!");
  _globalCommunicator = utils::Parallel::getGlobalCommunicator();
  _localCommunicator = utils::Parallel::getLocalCommunicator();
  MPI_Intercomm_create(
      _localCommunicator,
      0, // Local communicator, local leader rank
      _globalCommunicator,
      getLeaderRank(nameRequester), // Peer communicator, remote leader rank
      0,
      &communicator()); // Tag, intercommunicator to be created
  _isConnected = true;
}

void
MPIDirectCommunication::closeConnection() {
  preciceTrace("closeConnection()");

  if (not isConnected())
    return;

  MPI_Comm_free(&communicator());

  _isConnected = false;
}

void
MPIDirectCommunication::requestConnection(std::string const& nameAcceptor,
                                          std::string const& nameRequester,
                                          int requesterProcessRank,
                                          int requesterCommunicatorSize) {
  preciceTrace2("requestConnection()", nameAcceptor, nameRequester);
  assertion(not isConnected());

  utils::Parallel::splitCommunicator(nameRequester);

  preciceCheck(utils::Parallel::getCommunicatorSize() > 1,
               "requestConnection()",
               "ERROR: MPI communication direct (i.e. single) can be only "
                   << "used with more than one process in base communicator!");
  _globalCommunicator = utils::Parallel::getGlobalCommunicator();
  _localCommunicator = utils::Parallel::getLocalCommunicator();
  MPI_Intercomm_create(
      _localCommunicator,
      0, // Local communicator, local leader rank
      _globalCommunicator,
      getLeaderRank(nameAcceptor), // Peer communicator, remote leader rank
      0,
      &communicator()); // Tag, intercommunicator to be created
  _isConnected = true;
}

int
MPIDirectCommunication::getGroupID(std::string const& accessorName) {
  preciceTrace1("getGroupID()", accessorName);
  typedef utils::Parallel Par;
  const std::vector<Par::AccessorGroup>& _groups = Par::getAccessorGroups();
  for (const Par::AccessorGroup& group : _groups) {
    if (group.name == accessorName) {
      preciceDebug("return group ID = " << group.id);
      return group.id;
    }
  }
  preciceError("getGroupID()",
               "Unknown accessor name \"" << accessorName << "\"!");
}

int
MPIDirectCommunication::getLeaderRank(std::string const& accessorName) {
  preciceTrace1("getLeaderRank()", accessorName);
  typedef utils::Parallel Par;
  const std::vector<Par::AccessorGroup>& _groups = Par::getAccessorGroups();
  for (const Par::AccessorGroup& group : _groups) {
    if (group.name == accessorName) {
      preciceDebug("return rank = " << group.leaderRank);
      return group.leaderRank;
    }
  }
  preciceError("getLeaderRank()",
               "Unknown accessor name \"" << accessorName << "\"!");
}

void
MPIDirectCommunication::allreduceSum() {
  preciceTrace("allreduceSum()");
  // _comunicator did't work here as we seem to have two communicators, one with the master and one with the slaves
  MPI_Allreduce(nullptr, nullptr, 0, MPI_DATATYPE_NULL, MPI_OP_NULL, _globalCommunicator);
}

void
MPIDirectCommunication::reduceSum(double* itemsToSend, double* itemsToReceive, int size) {
  preciceTrace1("reduceSum(double*)", size);
  int rank = -1;
  MPI_Comm_rank ( _globalCommunicator, &rank );
  // _comunicator did't work here as we seem to have two communicators, one with the master and one with the slaves
  MPI_Reduce(itemsToSend, itemsToReceive, size, MPI_DOUBLE, MPI_SUM, rank, _globalCommunicator);
}

void
MPIDirectCommunication::reduceSum(double* itemsToSend, double* itemsToReceive, int size, int rankMaster) {
  preciceTrace1("reduceSum(double*)", size);
  // _comunicator did't work here as we seem to have two communicators, one with the master and one with the slaves
  MPI_Reduce(itemsToSend, itemsToReceive, size, MPI_DOUBLE, MPI_SUM, rankMaster, _globalCommunicator);
}

void
MPIDirectCommunication::reduceSum(int& itemsToSend, int& itemsToReceive) {
  preciceTrace("reduceSum(int)");
  int rank = -1;
  MPI_Comm_rank ( _globalCommunicator, &rank );
  // _comunicator did't work here as we seem to have two communicators, one with the master and one with the slaves
  MPI_Reduce(&itemsToSend, &itemsToReceive, 1, MPI_INT, MPI_SUM, rank, _globalCommunicator);
}

void
MPIDirectCommunication::reduceSum(int& itemsToSend, int& itemsToReceive, int rankMaster) {
  preciceTrace("reduceSum(double*)");
  // _comunicator did't work here as we seem to have two communicators, one with the master and one with the slaves
  MPI_Reduce(&itemsToSend, &itemsToReceive, 1, MPI_INT, MPI_SUM, rankMaster, _globalCommunicator);
}

void
MPIDirectCommunication::allreduceSum(double* itemsToSend, double* itemsToReceive, int size) {
  preciceTrace1("allreduceSum(double*)", size);
  // _comunicator did't work here as we seem to have two communicators, one with the master and one with the slaves
  MPI_Allreduce(itemsToSend, itemsToReceive, size, MPI_DOUBLE, MPI_SUM, _globalCommunicator);
}

void
MPIDirectCommunication::allreduceSum(double* itemsToSend, double* itemsToReceive, int size, int rankMaster) {
  preciceTrace1("allreduceSum(double*)", size);
  // _comunicator did't work here as we seem to have two communicators, one with the master and one with the slaves
  MPI_Allreduce(itemsToSend, itemsToReceive, size, MPI_DOUBLE, MPI_SUM, _globalCommunicator);
}

void
MPIDirectCommunication::allreduceSum(double& itemToSend, double& itemToReceive) {
  preciceTrace("allreduceSum(double)");
  // _comunicator did't work here as we seem to have two communicators, one with the master and one with the slaves
  MPI_Allreduce(&itemToSend, &itemToReceive, 1, MPI_DOUBLE, MPI_SUM, _globalCommunicator);
}

void
MPIDirectCommunication::allreduceSum(double& itemToSend, double& itemToReceive, int rankMaster) {
  preciceTrace("allreduceSum(double)");
  // _comunicator did't work here as we seem to have two communicators, one with the master and one with the slaves
  MPI_Allreduce(&itemToSend, &itemToReceive, 1, MPI_DOUBLE, MPI_SUM, _globalCommunicator);
}

void
MPIDirectCommunication::allreduceSum(int& itemToSend, int& itemToReceive) {
  preciceTrace("allreduceSum(double)");
  // _comunicator did't work here as we seem to have two communicators, one with the master and one with the slaves
  MPI_Allreduce(&itemToSend, &itemToReceive, 1, MPI_INT, MPI_SUM, _globalCommunicator);
}

void
MPIDirectCommunication::allreduceSum(int& itemToSend, int& itemToReceive, int rankMaster) {
  preciceTrace("allreduceSum(double)");
  // _comunicator did't work here as we seem to have two communicators, one with the master and one with the slaves
  MPI_Allreduce(&itemToSend, &itemToReceive, 1, MPI_INT, MPI_SUM, _globalCommunicator);
}

void
MPIDirectCommunication::broadcast() {
  preciceTrace("broadcast()");

  MPI_Bcast(nullptr, 0, MPI_DATATYPE_NULL, MPI_PROC_NULL, _communicator);
}

void
MPIDirectCommunication::broadcast(int* itemsToSend, int size) {
  preciceTrace1("broadcast(int*)", size);

  MPI_Bcast(itemsToSend, size, MPI_INT, MPI_ROOT, _communicator);
}

void
MPIDirectCommunication::broadcast(int* itemsToReceive,
                                  int size,
                                  int rankBroadcaster) {
  preciceTrace1("broadcast(int*)", size);

  MPI_Bcast(itemsToReceive, size, MPI_INT, rankBroadcaster, _communicator);
}

void
MPIDirectCommunication::broadcast(int itemToSend) {
  preciceTrace("broadcast(int)");

  broadcast(&itemToSend, 1);
}

void
MPIDirectCommunication::broadcast(int& itemToReceive, int rankBroadcaster) {
  preciceTrace("broadcast(int&)");

  broadcast(&itemToReceive, 1, rankBroadcaster);
}

void
MPIDirectCommunication::broadcast(double* itemsToSend, int size) {
  preciceTrace1("broadcast(double*)", size);

  MPI_Bcast(itemsToSend, size, MPI_DOUBLE, MPI_ROOT, _communicator);
}

void
MPIDirectCommunication::broadcast(double* itemsToReceive,
                                  int size,
                                  int rankBroadcaster) {
  preciceTrace1("broadcast(double*)", size);

  MPI_Bcast(itemsToReceive, size, MPI_DOUBLE, rankBroadcaster, _communicator);
}

void
MPIDirectCommunication::broadcast(double itemToSend) {
  preciceTrace("broadcast(double)");

  broadcast(&itemToSend, 1);
}

void
MPIDirectCommunication::broadcast(double& itemToReceive, int rankBroadcaster) {
  preciceTrace("broadcast(double&)");

  broadcast(&itemToReceive, 1, rankBroadcaster);
}

void
MPIDirectCommunication::broadcast(bool itemToSend) {
  preciceTrace("broadcast(bool)");

  int item = itemToSend;

  broadcast(item);
}

void
MPIDirectCommunication::broadcast(bool& itemToReceive, int rankBroadcaster) {
  preciceTrace("broadcast(bool&)");

  int item;

  broadcast(item, rankBroadcaster);

  itemToReceive = item;
}

MPI_Comm&
MPIDirectCommunication::communicator(int rank) {
  return _communicator;
}

int
MPIDirectCommunication::rank(int rank) {
  return rank;
}
}
} // close namespaces

#endif // not PRECICE_NO_MPI
