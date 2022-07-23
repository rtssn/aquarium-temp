public class Telemetry
{
    public List<Sensor>? sensors { get; set; }
    public int isFanOn { get; set; }
    public string? ipAddress { get; set; }

    public string? deviceId { get; set; }
}

public class Sensor
{
    public string? addoress { get; set; }
    public float temp { get; set; }
}
