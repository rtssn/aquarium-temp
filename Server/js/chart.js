/**
 * データの取得を行います。
 */
const getData = () => {
    fetch('./api/index.php')
        .then((response) => response.json())
        .then((data) => {
            const chartData = convertData(data);

            createChart(chartData.labels, chartData.sensor1TempData, chartData.sensor2TempData);

            createListTable(data);
            setNowTemp(data);

            const minTempElement = document.getElementById('min-temp');
            const minTempdatetimeElement = document.getElementById('min-temp-datetime');

            minTempElement.innerText = chartData.minTemp.temp;
            minTempdatetimeElement.innerText = chartData.minTemp.datetime;

            const maxTempElement = document.getElementById('max-temp');
            const maxTempdatetimeElement = document.getElementById('max-temp-datetime');

            maxTempElement.innerText = chartData.maxTemp.temp;
            maxTempdatetimeElement.innerText = chartData.maxTemp.datetime;
        });
};

/**
 * グラフの生成を行います。
 * @param {Array<string>} labels グラフのラベルを指定します。
 * @param {Array<float>} sensor1Data 室温のデータを指定します。
 * @param {Array<float>} sensor2Data 水温のデータを指定します。
 */
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

/**
 * 現在の水温・湿度を表示します。
 * @param {*} data
 */
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

/**
 * グラフ用にデータの変換を行います。
 */
const convertData = (data) => {
    const labels = [];
    const sensor1TempData = [];
    const sensor2TempData = [];

    let minTemp = Number.MAX_SAFE_INTEGER;
    let maxTemp = Number.MIN_SAFE_INTEGER;

    let minTempdatetime = '';
    let maxTempdatetime = '';

    data.forEach((element) => {
        labels.push(element.datetime);
        sensor1TempData.push(element.sensor1Temp);
        sensor2TempData.push(element.sensor2Temp);

        if (minTemp > element.sensor2Temp) {
            minTemp = element.sensor2Temp;
            minTempdatetime = element.datetime;
        } else if (maxTemp < element.sensor1Temp) {
            maxTemp = element.sensor2Temp;
            maxTempdatetime = element.datetime;
        }
    });

    const ret = {
        labels: labels,
        sensor1TempData: sensor1TempData,
        sensor2TempData: sensor2TempData,
        minTemp: {
            datetime: minTempdatetime,
            temp: minTemp,
        },
        maxTemp: {
            datetime: maxTempdatetime,
            temp: maxTemp,
        },
    };

    return ret;
};

const createListTable = (data) => {
    const table = document.getElementById('list');

    data.forEach((element) => {
        const datetime = element.datetime;
        const sensor1Temp = element.sensor1Temp;
        const sensor2Temp = element.sensor2Temp;
        const fanOn = element.isFanOn;

        const datetimeCell = createCol(datetime);
        const sensor1TempCell = createCol(sensor1Temp);
        const sensor2TempCell = createCol(sensor2Temp);

        const fanOnCell = createCol(sensor1Temp);

        if (fanOn == 1) {
            fanOnCell.innerText = 'ON';
        } else {
            fanOnCell.innerText = 'OFF';
        }

        const cols = [datetimeCell, sensor1TempCell, sensor2TempCell, fanOnCell];
        const row = createRow(cols);

        table.appendChild(row);
    });
};

/**
 * テーブルのカラムを生成します。
 * @param {} data
 * @returns 生成したカラム要素を返します。
 */
const createCol = (data) => {
    const ret = document.createElement('td');
    ret.innerText = data;

    return ret;
};

/**
 * テーブルの行を生成します。
 * @param {Array<HTMLTableCellElement>} cols
 * @returns 生成した行を返します。
 */
const createRow = (cols) => {
    const ret = document.createElement('tr');

    cols.forEach((col) => {
        ret.appendChild(col);
    });

    return ret;
};

getData();
