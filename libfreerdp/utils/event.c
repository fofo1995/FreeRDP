/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * Events
 *
 * Copyright 2011 Vic Lee
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <winpr/crt.h>

#include <freerdp/message.h>
#include <freerdp/utils/event.h>
#include <freerdp/client/cliprdr.h>
#include <freerdp/rail.h>

static wMessage* freerdp_cliprdr_event_new(UINT16 event_type)
{
	union
	{
		wMessage* m;
		void *v;
	} event;

	event.m = NULL;
	switch (event_type)
	{
		case CliprdrChannel_MonitorReady:
			event.v = malloc(sizeof(RDP_CB_MONITOR_READY_EVENT));
			ZeroMemory(event.v, sizeof(RDP_CB_MONITOR_READY_EVENT));
			event.m->id = MakeMessageId(CliprdrChannel, MonitorReady);
			break;

		case CliprdrChannel_FormatList:
			event.v = malloc(sizeof(RDP_CB_FORMAT_LIST_EVENT));
			ZeroMemory(event.v, sizeof(RDP_CB_FORMAT_LIST_EVENT));
			event.m->id = MakeMessageId(CliprdrChannel, FormatList);
			break;

		case CliprdrChannel_DataRequest:
			event.v = malloc(sizeof(RDP_CB_DATA_REQUEST_EVENT));
			ZeroMemory(event.v, sizeof(RDP_CB_DATA_REQUEST_EVENT));
			event.m->id = MakeMessageId(CliprdrChannel, DataRequest);
			break;

		case CliprdrChannel_DataResponse:
			event.v = malloc(sizeof(RDP_CB_DATA_RESPONSE_EVENT));
			ZeroMemory(event.v, sizeof(RDP_CB_DATA_RESPONSE_EVENT));
			event.m->id = MakeMessageId(CliprdrChannel, DataResponse);
			break;

		case CliprdrChannel_ClipCaps:
			event.v = malloc(sizeof(RDP_CB_CLIP_CAPS));
			ZeroMemory(event.v, sizeof(RDP_CB_CLIP_CAPS));
			event.m->id = MakeMessageId(CliprdrChannel, ClipCaps);
			break;

		case CliprdrChannel_FilecontentsRequest:
			event.v = (wMessage*) malloc(sizeof(RDP_CB_FILECONTENTS_REQUEST_EVENT));
			ZeroMemory(event.v, sizeof(RDP_CB_FILECONTENTS_REQUEST_EVENT));
			event.m->id = MakeMessageId(CliprdrChannel, FilecontentsRequest);
			break;

		case CliprdrChannel_FilecontentsResponse:
			event.v = (wMessage*) malloc(sizeof(RDP_CB_FILECONTENTS_RESPONSE_EVENT));
			ZeroMemory(event.v, sizeof(RDP_CB_FILECONTENTS_RESPONSE_EVENT));
			event.m->id = MakeMessageId(CliprdrChannel, FilecontentsResponse);
			break;

		case CliprdrChannel_LockClipdata:
			event.v = (wMessage*) malloc(sizeof(RDP_CB_LOCK_CLIPDATA_EVENT));
			ZeroMemory(event.v, sizeof(RDP_CB_LOCK_CLIPDATA_EVENT));
			event.m->id = MakeMessageId(CliprdrChannel, LockClipdata);
			break;

		case CliprdrChannel_UnLockClipdata:
			event.v = (wMessage*) malloc(sizeof(RDP_CB_UNLOCK_CLIPDATA_EVENT));
			ZeroMemory(event.v, sizeof(RDP_CB_UNLOCK_CLIPDATA_EVENT));
			event.m->id = MakeMessageId(CliprdrChannel, UnLockClipdata);
			break;

		case CliprdrChannel_TemporaryDirectory:
			event.v = (wMessage*) malloc(sizeof(RDP_CB_TEMPDIR_EVENT));
			ZeroMemory(event.v, sizeof(RDP_CB_TEMPDIR_EVENT));
			event.m->id = MakeMessageId(CliprdrChannel, TemporaryDirectory);
			break;

	}

	return event.m;
}

static wMessage* freerdp_rail_event_new(UINT16 event_type)
{
	wMessage* event = NULL;

	event = (wMessage*) malloc(sizeof(wMessage));
	ZeroMemory(event, sizeof(wMessage));

	return event;
}

wMessage* freerdp_event_new(UINT16 event_class, UINT16 event_type,
	MESSAGE_FREE_FN on_event_free_callback, void* user_data)
{
	wMessage* event = NULL;

	switch (event_class)
	{
		case CliprdrChannel_Class:
			event = freerdp_cliprdr_event_new(event_type);
			break;

		case RailChannel_Class:
			event = freerdp_rail_event_new(event_type);
			break;
	}

	if (event)
	{
		event->wParam = user_data;
		event->Free = (void*) on_event_free_callback;
		event->id = GetMessageId(event_class, event_type);
	}

	return event;
}

static void freerdp_cliprdr_event_free(wMessage* event)
{
	switch (GetMessageType(event->id))
	{
		case CliprdrChannel_FormatList:
			{
				RDP_CB_FORMAT_LIST_EVENT* cb_event = (RDP_CB_FORMAT_LIST_EVENT*) event;
				free(cb_event->formats);
				free(cb_event->raw_format_data);
			}
			break;

		case CliprdrChannel_DataResponse:
			{
				RDP_CB_DATA_RESPONSE_EVENT* cb_event = (RDP_CB_DATA_RESPONSE_EVENT*) event;
				free(cb_event->data);
			}
			break;
	}
}

static void freerdp_rail_event_free(wMessage* event)
{

}

void freerdp_event_free(wMessage* event)
{
	if (event)
	{
		if (event->Free)
			event->Free(event);

		switch (GetMessageClass(event->id))
		{
			case CliprdrChannel_Class:
				freerdp_cliprdr_event_free(event);
				break;

			case RailChannel_Class:
				freerdp_rail_event_free(event);
				break;
		}

		free(event);
	}
}
