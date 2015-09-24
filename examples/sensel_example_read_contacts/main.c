/**************************************************************************
 * Copyright 2015 Sensel, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **************************************************************************/
/**
 * Read Contacts
 * by Aaron Zarraga - Sensel, Inc
 * 
 * This opens a Sensel sensor, reads contact data, and prints the data to the console.
 */

#include <stdio.h>
#include <signal.h> //so we can catch a ctrl+c key event
#include "sensel.h"

volatile sig_atomic_t ctrl_c_requested = false;

contact_t contacts[MAX_CONTACTS];
int n_contacts = 0;

void handle_ctrl_c(int sig)
{
  ctrl_c_requested = true;
}

int main()
{
  signal (SIGINT, handle_ctrl_c);

  bool sensel_sensor_opened = false;

  sensel_sensor_opened = senselOpenConnection(0);
  
  if(!sensel_sensor_opened)
  {
    printf("Unable to open Sensel sensor!\n");
    return -1;
  }
  
  //Enable contact sending
  senselSetFrameContentControl(SENSEL_FRAME_CONTACTS_FLAG);
  
  //Enable scanning
  senselStartScanning();

  while(!ctrl_c_requested)
  {
    n_contacts = senselReadContacts(contacts);
  
    if(n_contacts == 0)
    {
      printf("NO CONTACTS\n");
    }
   
    for(int i = 0; i < n_contacts; i++)
    {
      int force = contacts[i].total_force;
      float sensor_x = contacts[i].x_pos;
      float sensor_y = contacts[i].y_pos;
      
      int id = contacts[i].id;
      int event_type = contacts[i].type;
      
      char* event;
      switch (event_type)
      {
        case SENSEL_EVENT_CONTACT_INVALID:
          event = "invalid";
          break;
        case SENSEL_EVENT_CONTACT_START:
          event = "start";
          break;
        case SENSEL_EVENT_CONTACT_MOVE:
          event = "move";
          break;
        case SENSEL_EVENT_CONTACT_END:
          event = "end";
          break;
        default:
          event = "error";
      }
      
      printf("Contact ID %d, event=%s, mm coord: (%f, %f), force=%d\n", 
             id, event, sensor_x, sensor_y, force);
    }
    
    if(n_contacts > 0)
      printf("****\n");
  }

  printf("Closing application\n");

  if(sensel_sensor_opened)
  {
    senselStopScanning();
    senselCloseConnection();
  }
}

