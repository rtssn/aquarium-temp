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

            if (chart === undefined) {
                chart = createChart(chartData.labels, chartData.sensor1TempData, chartData.sensor2TempData);
                timer = setInterval(getNowData, 60 * 1000, chart);
            } else {
                chart.data.datasets[0].data = chartData.sensor1TempData;
                chart.data.datasets[1].data = chartData.sensor2TempData;
                chart.update();
            }

            createListTable(data.list);

            const length = data.list.length;
            const now = data.list[length - 1];

            setNowTemp(now);
            setMinMaxTemp(data.minTemp, data.maxTemp);
        });
};

/**
 * 現在の水温と室温を取得します。
 * @param {*} chart
 */
const getNowData = (chart) => {
    fetch('./api/index.php?now')
        .then((response) => response.json())
        .then((data) => {
            chart.data.datasets[0].data.push(data.now.sensor1TempData);
            chart.data.datasets[1].data.push(data.now.sensor2TempData);
            chart.data.labels.push(data.now.datetime);

            delete chart.data.datasets[0].data[0];
            delete chart.data.datasets[0].data[1];
            delete chart.data.labels[0];

            chart.update();

            addNowDataToTable(data.now);
            setNowTemp(data.now);
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
 * 現在の水温・室温を表示します。
 * @param {*} now 現在の水温・室温を指定します。
 */
const setNowTemp = (now) => {
    const nowElement = document.getElementById('now');
    const nowTankTempElement = document.getElementById('now-tank-temp');
    const nowRoomTempElement = document.getElementById('now-room-temp');
    const nowFanElement = document.getElementById('now-fan');

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

    data.forEach((record) => {
        const cols = createCols(record);
        const row = createRow(cols);
        fragment.appendChild(row);
    });

    table.appendChild(fragment);
};

/**
 * テーブルに列を追加します。
 * @param {*} now 現在のデータを指定します。
 */
const addNowDataToTable = (now) => {
    const cols = createCols(now);
    const row = createRow(cols);

    const table = document.getElementById('list');
    table.appendChild(row);
};

/**
 * テーブルのカラム生成を行います。
 * @param {*} record データの1行を指定します。
 * @returns 生成したカラムを返します。
 */
const createCols = (record) => {
    const datetime = record.datetime;
    const sensor1Temp = record.sensor1Temp;
    const sensor2Temp = record.sensor2Temp;
    const fanOn = record.isFanOn;

    const datetimeCol = createCol(datetime);
    const sensor1TempCol = createCol(sensor1Temp);
    const sensor2TempCol = createCol(sensor2Temp);

    const fanOnCol = createCol(sensor1Temp);

    if (fanOn == 1) {
        fanOnCol.innerText = 'ON';
    } else {
        fanOnCol.innerText = 'OFF';
    }

    const cols = [datetimeCol, sensor1TempCol, sensor2TempCol, fanOnCol];

    return cols;
};

/**
 * テーブルのカラムタグを生成します。
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
