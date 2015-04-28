/*
 * MTimer.cxx
 *
 *
 * Copyright (C) by Andreas Zoglauer.
 * All rights reserved.
 *
 *
 * This code implementation is the intellectual property of
 * Andreas Zoglauer.
 *
 * By copying, distributing or modifying the Program (or any work
 * based on the Program) you indicate your acceptance of this statement,
 * and all its terms.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// MTimer
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "MTimer.h"

// Standard libs:
#include <iostream>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MSystem.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(MTimer)
#endif


////////////////////////////////////////////////////////////////////////////////


MTimer::MTimer(bool Start)
{
  // Default constructor

  m_StartTime = GetRelativeTime();
  m_ElapsedTime = 0;

  m_HasTimedOut = false;
  m_TimeOut = 0;

  m_IsPaused = !Start;
}


////////////////////////////////////////////////////////////////////////////////


MTimer::MTimer(double TimeOut)
{
  // Standard constrcutor

  m_StartTime = GetRelativeTime();
  m_ElapsedTime = 0;

  m_HasTimedOut = false;
  m_TimeOut = TimeOut;

  m_IsPaused = false;
}


////////////////////////////////////////////////////////////////////////////////


MTimer::MTimer(const MTimer& Timer)
{
  // Copy constructor
  
  lock_guard<mutex> lock(Timer.m_Guard);
 
  m_StartTime = Timer.m_StartTime;
  m_HasTimedOut = Timer.m_HasTimedOut;
  m_TimeOut = Timer.m_TimeOut;
  m_ElapsedTime = Timer.m_ElapsedTime;
  m_IsPaused = Timer.m_IsPaused;
}


////////////////////////////////////////////////////////////////////////////////


MTimer::~MTimer()
{
  // Default destructor
}


////////////////////////////////////////////////////////////////////////////////


MTimer& MTimer::operator=(const MTimer& Timer)
{
  // Copy constructor
  
  lock_guard<mutex> lock(m_Guard);
 
  if (this != &Timer) {
    lock_guard<mutex> lock2(Timer.m_Guard);
    
    m_StartTime = Timer.m_StartTime;
    m_HasTimedOut = Timer.m_HasTimedOut;
    m_TimeOut = Timer.m_TimeOut;
    m_ElapsedTime = Timer.m_ElapsedTime;
    m_IsPaused = Timer.m_IsPaused;
  }
  
  return *this;
}


////////////////////////////////////////////////////////////////////////////////


void MTimer::Clear()
{
  // (Re-) Start the timer

  lock_guard<mutex> lock(m_Guard);
  
  m_IsPaused = true;

  m_StartTime = 0;
  m_HasTimedOut = false;
  m_TimeOut = 0;
  m_ElapsedTime = 0;
}


////////////////////////////////////////////////////////////////////////////////


void MTimer::Start()
{
  // (Re-) Start the timer

  lock_guard<mutex> lock(m_Guard);
  
  m_StartTime = GetRelativeTime();
  m_HasTimedOut = false;
  m_ElapsedTime = 0;
  m_IsPaused = false;
}


////////////////////////////////////////////////////////////////////////////////


void MTimer::Reset()
{
  // (Re-) Start the timer --- same as start

  Start();
}


////////////////////////////////////////////////////////////////////////////////


void MTimer::Pause()
{
  // (Re-) Start the timer

  lock_guard<mutex> lock(m_Guard);
  
  if (m_IsPaused == true) return;
  
  m_ElapsedTime = GetElapsedTime();
  m_IsPaused = true;
}


////////////////////////////////////////////////////////////////////////////////


void MTimer::Continue()
{
  // Start the timer after it has been paused - does not reset the time

  lock_guard<mutex> lock(m_Guard);

  if (m_IsPaused == false) return;
  
  m_StartTime = GetRelativeTime();
  m_IsPaused = false;
}


////////////////////////////////////////////////////////////////////////////////


double MTimer::GetRelativeTime()
{
  // Convert the time to seconds and return as double
  
  long int Seconds = 0, Nanoseconds = 0;
  MSystem::GetTime(Seconds, Nanoseconds);

  return Nanoseconds/1000000000.0 + Seconds;
}


////////////////////////////////////////////////////////////////////////////////


void MTimer::SetTimeOut(double TimeOut) 
{ 
  // Set the timeout
  // If timeout <= 0 then never timeout

  lock_guard<mutex> lock(m_Guard);
  
  m_TimeOut = TimeOut;
}
 

////////////////////////////////////////////////////////////////////////////////


double MTimer::GetTimeOut()
{
  // Return the timeout
  // If timeout <= 0 then never timeout

  lock_guard<mutex> lock(m_Guard);

  return m_TimeOut;
}


////////////////////////////////////////////////////////////////////////////////


double MTimer::GetElapsed()
{
  // Convert the time to seconds and return as double

  lock_guard<mutex> lock(m_Guard);

  return GetElapsedTime();
}


////////////////////////////////////////////////////////////////////////////////


double MTimer::GetElapsedTime()
{
  // Convert the time to seconds and return as double

  if (m_IsPaused == true) {
    return m_ElapsedTime;
  } else {
    return GetRelativeTime() - m_StartTime + m_ElapsedTime;
  }
}


////////////////////////////////////////////////////////////////////////////////


bool MTimer::HasTimedOut(double Seconds)
{
  // Convert the time to seconds and return as double

  lock_guard<mutex> lock(m_Guard);

  if (m_HasTimedOut == true) return true;

  if (m_TimeOut <= 0) return false;

  if (Seconds == -1) Seconds = m_TimeOut;

  if (GetRelativeTime() - m_StartTime > Seconds) return true;

  return false;
}


// MTimer.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
