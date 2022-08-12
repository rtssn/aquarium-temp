const main = () => {
    getData();
};

/**
 * データの取得を行います。
 */
const getData = (chart) => {
    fetch('./api/index.php')
        .then((response) => response.json())
        .then((data) => {
            const chartData = convertData(data.list);

            console.log(chartData);

            if (chart === undefined) {
                chart = createChart(chartData.labels, chartData.sensor1TempData, chartData.sensor2TempData);
                timer = setInterval(getData, 60 * 1000, chart);
            } else {
                chart.data.datasets[0].data = chartData.sensor1TempData;
                chart.data.datasets[1].data = chartData.sensor2TempData;
                chart.update();
            }

            createListTable(data.list);
            setNowTemp(data.list);
            setMinMaxTemp(data.minTemp, data.maxTemp);
        });
};

/**
 * グラフの生成を行います。
 * @param {Array<string>} labels グラフのラベルを指定します。
 * @param {Array<number>} sensor1Data 室温のデータを指定します。
 * @param {Array<number>} sensor2Data 水温のデータを指定します。
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

    const chart = new Chart(document.getElementById('myChart'), config);

    return chart;
};

/**
 * 現在の水温・湿度を表示します。
 * @param {*} data 取得したデータを指定します。
 */
const setNowTemp = (data) => {
    const nowElement = document.getElementById('now');
    const nowTankTempElement = document.getElementById('now-tank-temp');
    const nowRoomTempElement = document.getElementById('now-room-temp');
    const nowFanElement = document.getElementById('now-fan');

    const length = data.length;
    const now = data[length - 1];

    nowElement.innerText = now.datetime;
    nowTankTempElement.innerText = now.sensor2Temp;
    nowRoomTempElement.innerText = now.sensor1Temp;

    if (now.isFanOn == 1) {
        nowFanElement.innerText = 'ON';
    } else {
        nowFanElement.innerText = 'OFF';
    }
};

/**
 * 最低水温、最高水温を表示します。
 * @param {*} minTemp 最低水温と最低水温を記録した日時が格納されたオブジェクトを指定します。
 * @param {*} maxTemp 最高水温と最高水温を記録した日時が格納されたオブジェクトを指定します。
 */
const setMinMaxTemp = (minTemp, maxTemp) => {
    const minTempElement = document.getElementById('min-temp');
    const minTempdatetimeElement = document.getElementById('min-temp-datetime');

    minTempElement.innerText = minTemp.temp;
    minTempdatetimeElement.innerText = minTemp.datetime;

    const maxTempElement = document.getElementById('max-temp');
    const maxTempdatetimeElement = document.getElementById('max-temp-datetime');

    maxTempElement.innerText = maxTemp.temp;
    maxTempdatetimeElement.innerText = maxTemp.datetime;
};

/**
 * グラフ用にデータの変換を行います。
 * @param {*} 取得したデータを指定します。
 */
const convertData = (data) => {
    const labels = [];
    const sensor1TempData = [];
    const sensor2TempData = [];

    data.forEach((element) => {
        labels.push(element.datetime);
        sensor1TempData.push(element.sensor1Temp);
        sensor2TempData.push(element.sensor2Temp);
    });

    const ret = {
        labels: labels,
        sensor1TempData: sensor1TempData,
        sensor2TempData: sensor2TempData,
    };

    return ret;
};

/**
 * データ一覧表を生成します。
 * @param {*} data 取得したデータを指定します。
 */
const createListTable = (data) => {
    const table = document.getElementById('list');
    const fragment = new DocumentFragment();

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
        fragment.appendChild(row);
    });

    table.appendChild(fragment);
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

main();
