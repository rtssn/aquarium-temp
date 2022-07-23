using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace Client
{
    internal class DeviceClient
    {
        /// <summary>
        /// 実験するクライアント数です。
        /// </summary>
        private const int clientCount = 100;

        /// <summary>
        /// クライアントのデバイスIDを格納するディクショナリです。
        /// </summary>
        private Dictionary<int, string> deviceIds = new Dictionary<int, string>();

        /// <summary>
        /// 仮想デバイスクライアントのコンストラクタです。
        /// デバイスIDの初期化を行います。
        /// </summary>
        public DeviceClient()
        {
            CreateDeviceId();
        }

        /// <summary>
        /// 負荷実験処理を実行します。
        /// 
        /// </summary>
        public void Run()
        {
            for (int i = 0; i < clientCount; i++)
            {
                Post(i);
            }
        }

        /// <summary>
        /// 送信処理を非同期で行います。
        /// </summary>
        private async void Post(int index)
        {
            Random rand = new Random();
            string deviceId = deviceIds[index];
            string ipAddress = $"192.168.11.{index}";

            List<Sensor> sensors = new List<Sensor>()
            {
                new Sensor()
                {
                    address = "test1",
                    temp = rand.Next(20, 30),
                },
                new Sensor()
                {
                    address = "test2",
                    temp = rand.Next(20, 30),
                }
            };

            Telemetry data = new Telemetry();

            data.ipAddress = ipAddress;
            data.deviceId = deviceId;
            data.isFanOn = 0;
            data.sensors = sensors;

            HttpClient client = new HttpClient();
            string jsonString = JsonSerializer.Serialize(data);
            StringContent content = new StringContent(jsonString, Encoding.UTF8, "application/json");

            HttpResponseMessage result = await client.PostAsync("https://test.solarsail.work/iot/api/index.php", content);
            Console.WriteLine($"{(int)result.StatusCode} {result.StatusCode}");
            Console.WriteLine(result.StatusCode);
        }


        /// <summary>
        /// デバイスIDの生成を行います。
        /// </summary>
        private void CreateDeviceId()
        {
            for (int i = 0; i < clientCount; i++)
            {
                Guid guid = Guid.NewGuid();
                string uuid = guid.ToString();
                uuid = uuid.Replace("{", string.Empty);
                uuid = uuid.Replace("}", string.Empty);
                uuid = uuid.Replace("-", string.Empty);

                deviceIds.Add(i, uuid);
            }
        }
    }
}
