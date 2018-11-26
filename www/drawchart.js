var xhr = new XMLHttpRequest();
xhr.open("GET", dataPortalHost + "/getdata", true);
xhr.setRequestHeader("Content-Type", "application/json");
xhr.onreadystatechange = function () {
    if (xhr.readyState === 4 && xhr.status === 200) {
        var chartData = JSON.parse(xhr.responseText);

        chartData = chartData
            .map(e => {
                return {
                    "t": e.timestamp.slice(0, 10) + "T" + e.timestamp.slice(11) + "+0000",
                    "y": e.value
                }
            });

        var ctx = document.getElementById('canvas').getContext('2d');
        var config = {
            type: "line",
            data: {
                datasets: [{
                    label: 'Moisture data',
                    data: chartData,
                    fill: false,
                }]
            },
            options: {
                scales: {
                    xAxes: [{
                        type: 'time',
                        distribution: 'linear',
                        time: {
                            isoWeekday: true,
                            displayFormats: {
                                hour: 'H',
                                minute: 'H:mm'
                            }
                        }
                    }],
                    yAxes: [{
                        ticks: {
                            min: 0,
                            max: 255
                        }
                    }]
                },
                legend: {
                    display: false
                },
                title: {
                    text: "Soil moisture readings",
                    display: true
                }
            }
        };

        window.myLine = new Chart(ctx, config);
    }
};

xhr.send();
