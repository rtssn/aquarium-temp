const getData = () => {
    fetch('./api/index.php')
        .then((response) => response.json())
        .then((data) => {
            const nowElement = document.getElementById('now');
            const nowTempElement = document.getElementById('now-temp');

            const length = data.length;

            nowTempElement.innerText = data[length - 1].temp;
            nowElement.innerText = data[length - 1].datetime;

            const labels = [];
            const tempdata = [];

            data.forEach((element) => {
                labels.push(element.datetime);
                tempdata.push(element.temp);
            });

            const chartData = {
                labels: labels,
                datasets: [
                    {
                        label: '水槽水温',
                        backgroundColor: 'rgb(255, 99, 132)',
                        borderColor: 'rgb(255, 99, 132)',
                        data: tempdata,
                    },
                ],
            };

            const config = {
                type: 'line',
                data: chartData,
                options: {
                    spanGaps: true,
                },
            };

            const myChart = new Chart(document.getElementById('myChart'), config);
        });
};

getData();
