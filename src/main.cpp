// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "stdafx.h"

#include <Windows.h>

#include "RenderApp.h"

int APIENTRY WinMain( HINSTANCE hinstance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd )
{
#if defined( DEBUG ) | defined( _DEBUG )
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	try
	{
		RenderApp app( hinstance, cmd_line );

		if ( ! app.Initialize() )
			return 1;
		return app.Run();
	}
	catch ( DxException& e )
	{
		MessageBox( nullptr, e.ToString().c_str(), L"HR Failed", MB_OK );
		return 0;
	}
}