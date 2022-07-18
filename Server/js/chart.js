const getData = () => {
    fetch('./api/index.php')
        .then((response) => response.json())
        .then((data) => {
            const [labels, sensor1TempData, sensor2TempData, minTemp, minTempDatetime, maxTemp, maxTempDatetime] = convertData(data);

            createChart(labels, sensor1TempData, sensor2TempData);

            setNowTemp(data);

            const minTempElement = document.getElementById('min-temp');
            const minTempDatetimeElement = document.getElementById('min-temp-datetime');

            minTempElement.innerText = minTemp;
            minTempDatetimeElement.innerText = minTempDatetime;

            const maxTempElement = document.getElementById('max-temp');
            const maxTempDatetimeElement = document.getElementById('max-temp-datetime');

            maxTempElement.innerText = maxTemp;
            maxTempDatetimeElement.innerText = maxTempDatetime;
        });
};

const createChart = (labels, sensor1Data, sensor2Data) => {
    const chartData = {
        labels: labels,
        datasets: [
            {
                label: '室温',
                backgroundColor: 'rgb(255, 99, 132)',
                borderColor: 'rgb(255, 99, 132)',
                data: sensor1Data,
            },
            {
                label: '水温',
                backgroundColor: 'rgb(132, 99, 255)',
                borderColor: 'rgb(132, 99, 255)',
                data: sensor2Data,
            },
        ],
    };

    const config = {
        type: 'line',
        data: chartData,
        options: {
            spanGaps: true,
            maintainAspectRatio: false,
            elements: {
                point: {
                    radius: 0,
                },
            },
        },
    };

    const myChart = new Chart(document.getElementById('myChart'), config);
};

const setNowTemp = (data) => {
    const nowElement = document.getElementById('now');
    const nowTankTempElement = document.getElementById('now-tank-temp');
    const nowRoomTempElement = document.getElementById('now-room-temp');

    const length = data.length;

    const now = data[length - 1];

    nowElement.innerText = now.datetime;
    nowTankTempElement.innerText = now.sensor2Temp;
    nowRoomTempElement.innerText = now.sensor1Temp;
};

const convertData = (data) => {
    const labels = [];
    const sensor1TempData = [];
    const sensor2TempData = [];

    let minTemp = Number.MAX_SAFE_INTEGER;
    let maxTemp = Number.MIN_SAFE_INTEGER;

    let minTempDatetime = '';
    let maxTempDatetime = '';

    data.forEach((element) => {
        labels.push(element.datetime);
        sensor1TempData.push(element.sensor1Temp);
        sensor2TempData.push(element.sensor2Temp);

        if (minTemp > element.sensor2Temp) {
            minTemp = element.sensor2Temp;
            minTempDatetime = element.datetime;
        } else if (maxTemp < element.sensor1Temp) {
            maxTemp = element.sensor2Temp;
            maxTempDatetime = element.datetime;
        }
    });

    return [labels, sensor1TempData, sensor2TempData, minTemp, minTempDatetime, maxTemp, maxTempDatetime];
};

getData();
