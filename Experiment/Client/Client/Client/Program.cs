using Client;

Console.Write("start!");

DeviceClient deviceClient = new DeviceClient();

deviceClient.Run();

string? input = Console.ReadLine();