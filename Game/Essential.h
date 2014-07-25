#pragma once

#ifndef NULL
#define NULL 0
#endif

#define SAFE_DELETE(obj) {if(obj!=NULL){delete obj; obj = NULL;}}
#define SAFE_DELETE_ARR(obj) {if(obj!=NULL){delete[] obj; obj = NULL;}}
#define SAFE_RELEASE(obj) {if(obj!=NULL){obj->Release(); obj=NULL;}}

#define FPS 60