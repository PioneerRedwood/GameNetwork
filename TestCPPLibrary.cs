using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;

public class TestCPPLibrary : MonoBehaviour
{
    [DllImport("DLL_TEST_VER01")]
    public static extern float Devide(float a, float b);

    [DllImport("DLL_TEST_VER01")]
    public static extern void RunThread(int times);

    [DllImport("NetworkClient_Ver01")]
    public static extern bool Init(int interval, int port);

    [DllImport("NetworkClient_Ver01")]
    public static extern bool Connect();

    [DllImport("NetworkClient_Ver01")]
    public static extern string PopDataInDeque();

    [DllImport("NetworkClient_Ver01")]
    public static extern bool Update();

    // Start is called before the first frame update
    void Start()
    {
        if (Init(3, 9000))
        {
            InvokeRepeating(nameof(ClientUpdate), 0.0f, 2.0f);
        }
        else
        {
            Debug.Log("Init() failed");
        }
    }

    void ClientUpdate()
    {
        if(Connect())
        {
            if (Update())
            {
                Debug.Log(PopDataInDeque());
            }
            else
            {
                Debug.Log("Client Update() failed");
            }
        }
        else
        {
            Debug.Log("Client Connect() failed");
        }
    }
}
