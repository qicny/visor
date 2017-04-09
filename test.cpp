#include <assert.h>
#include <math.h>
#include <windows.h>
#include "graphics.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  if(msg == WM_CLOSE)
  {
    DestroyWindow(hwnd);
    return 0;
  }
  if(msg == WM_DESTROY)
  {
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hwnd, msg, wParam, lParam);
}

HWND create_window(HINSTANCE inst, LONG w, LONG h)
{
  const wchar_t *name = L"VisorTestProgram";

  WNDCLASSEX wc;
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = 0;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = inst;
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = name;
  wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

  if(!RegisterClassEx(&wc))
    assert(false);

  RECT wr = {0, 0, w, h};
  AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
  HWND wnd =
      CreateWindowEx(WS_EX_CLIENTEDGE, name, name, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                     CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, inst, NULL);

  assert(wnd);

  return wnd;
}

Image *LoadTexture(const char *filename)
{
  FILE *f = fopen(filename, "rb");
  if(!f)
    return NULL;

  char header[256];
  char *line = fgets(header, 256, f);
  if(!line || strncmp(header, "P6\n", 3))
  {
    fclose(f);
    return NULL;
  }

  do
  {
    line = fgets(header, 256, f);
    if(!line)
    {
      fclose(f);
      return NULL;
    }
  } while(!strncmp(header, "#", 1));

  uint32_t width = 1, height = 1;
  sscanf(header, "%u %u", &width, &height);
  if(width < 1 || width > 2048 || height < 1 || height > 2048)
  {
    fclose(f);
    return NULL;
  }

  char *format = fgets(header, 256, f);
  if(!format || strncmp(header, "255\n", 3))
  {
    fclose(f);
    return NULL;
  }

  byte *rgba = new byte[width * height * 4];

  for(uint32_t y = 0; y < height; y++)
  {
    uint8_t *rowPtr = rgba;

    for(uint32_t x = 0; x < width; x++)
    {
      fread(rowPtr, 3, 1, f);
      rowPtr += 4;
    }

    rgba += width * 4;
  }

  rgba -= width * height * 4;

  fclose(f);
  Image *ret = MakeImage(width, height, rgba);
  delete[] rgba;
  return ret;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  HWND wnd = create_window(hInstance, 500, 500);

  ShowWindow(wnd, SW_SHOW);
  UpdateWindow(wnd);

  Swapchain *swap = CreateSwapchain(wnd, 2);
  Image *images[2] = {
      GetImage(swap, 0), GetImage(swap, 1),
  };

  Image *tex = LoadTexture("lunarg.ppm");

  float time = 0.0f;

  MSG msg = {};
  while(true)
  {
    // Check to see if any messages are waiting in the queue
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      // Translate the message and dispatch it to WindowProc()
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    // If the message is WM_QUIT, exit the while loop
    if(msg.message == WM_QUIT)
      break;

    int bbidx = Acquire(swap);

    // data from vkcube
    float MVP[] = {
        -0.58406f, 0.00f,     -2.34251f, 0.00f,    1.20521f,  -2.07017f, -0.30049f, 0.00f,
        -0.83369f, -0.51553f, 0.20786f,  5.64243f, -0.83203f, -0.5145f,  0.20745f,  5.83095f,
    };

    float pos[] = {
        -1.00f, -1.00f, -1.00f, +1.00f, -1.00f, -1.00f, +1.00f, +1.00f, -1.00f, +1.00f, +1.00f,
        +1.00f, -1.00f, +1.00f, +1.00f, +1.00f, -1.00f, +1.00f, -1.00f, +1.00f, -1.00f, -1.00f,
        -1.00f, +1.00f, -1.00f, -1.00f, -1.00f, +1.00f, +1.00f, +1.00f, -1.00f, +1.00f, +1.00f,
        -1.00f, -1.00f, +1.00f, -1.00f, -1.00f, -1.00f, +1.00f, -1.00f, +1.00f, -1.00f, +1.00f,
        +1.00f, +1.00f, -1.00f, +1.00f, -1.00f, -1.00f, -1.00f, +1.00f, +1.00f, -1.00f, -1.00f,
        +1.00f, +1.00f, -1.00f, +1.00f, +1.00f, -1.00f, -1.00f, -1.00f, +1.00f, +1.00f, -1.00f,
        +1.00f, +1.00f, -1.00f, -1.00f, +1.00f, +1.00f, -1.00f, +1.00f, -1.00f, +1.00f, -1.00f,
        +1.00f, +1.00f, +1.00f, +1.00f, +1.00f, +1.00f, +1.00f, -1.00f, +1.00f, -1.00f, +1.00f,
        +1.00f, +1.00f, +1.00f, +1.00f, +1.00f, +1.00f, -1.00f, +1.00f, +1.00f, +1.00f, -1.00f,
        +1.00f, +1.00f, +1.00f, +1.00f, +1.00f, +1.00f, -1.00f, +1.00f, +1.00f, +1.00f, -1.00f,
        +1.00f, +1.00f, +1.00f, -1.00f, -1.00f, +1.00f, +1.00f, +1.00f, -1.00f, +1.00f, -1.00f,
        +1.00f, +1.00f, +1.00f, -1.00f, -1.00f, +1.00f, +1.00f, +1.00f, +1.00f, +1.00f, +1.00f,
        -1.00f, -1.00f, +1.00f, +1.00f, +1.00f, -1.00f, +1.00f, +1.00f, +1.00f, +1.00f, +1.00f,
        +1.00f,
    };

    float UV[] = {
        0.00f, 1.00f, 0.00f, 0.00f, 1.00f, 1.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f,
        1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f,
        1.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f,
        1.00f, 1.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f,
        1.00f, 0.00f, 0.00f, 0.00f, 1.00f, 1.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f,
        1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f,
        1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f,
        1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 1.00f, 1.00f, 0.00f, 0.00f,
        1.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f,
        0.00f, 1.00f, 0.00f, 0.00f, 1.00f, 1.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f,
        0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f,
        0.00f, 1.00f, 0.00f, 0.00f, 1.00f, 1.00f, 0.00f, 0.00f, 1.00f, 0.00f, 0.00f, 0.00f,
    };

    {
      MICROPROFILE_SCOPEI("test", "DrawTriangle", MP_YELLOW);

      DrawTriangle(images[bbidx], 36, pos, UV, MVP, tex);
    }

    Present(swap, bbidx);

    time += 0.01f;

    MicroProfileFlip(NULL);
  }

  Destroy(swap);

  DestroyWindow(wnd);

  return 0;
}