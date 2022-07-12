const getData = () => {
    fetch('./api/index.php')
        .then((response) => response.json())
        .then((data) => {
            const [labels, tempData, minTemp, minTempDatetime, maxTemp, maxTempDatetime] = convertData(data);

            createChart(labels, tempData);

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

const createChart = (labels, tempData) => {
    const chartData = {
        labels: labels,
        datasets: [
            {
                label: '水槽水温',
                backgroundColor: 'rgb(255, 99, 132)',
                borderColor: 'rgb(255, 99, 132)',
                data: tempData,
            },
        ],
    };

    const config = {
        type: 'line',
        data: chartData,
        options: {
            spanGaps: true,
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
    const nowTempElement = document.getElementById('now-temp');

    const length = data.length;

    nowTempElement.innerText = data[length - 1].temp;
    nowElement.innerText = data[length - 1].datetime;
};

const convertData = (data) => {
    const labels = [];
    const tempData = [];

    let minTemp = Number.MAX_SAFE_INTEGER;
    let maxTemp = Number.MIN_SAFE_INTEGER;

    let minTempDatetime = '';
    let maxTempDatetime = '';

    data.forEach((element) => {
        labels.push(element.datetime);
        tempData.push(element.temp);

        if (minTemp > element.temp) {
            minTemp = element.temp;
            minTempDatetime = element.datetime;
        } else if (maxTemp < element.temp) {
            maxTemp = element.temp;
            maxTempDatetime = element.datetime;
        }
    });

    return [labels, tempData, minTemp, minTempDatetime, maxTemp, maxTempDatetime];
};

getData();
