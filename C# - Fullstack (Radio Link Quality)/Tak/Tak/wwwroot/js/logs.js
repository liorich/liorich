function loggerProcessDates(ob) {
    var Arr = ob;
    var selector = document.getElementById("logDates");
    selector.innerHTML = "<option value='none'>Select Date:</option>";
    Arr.forEach((date) => {
        var option = document.createElement("option");
        option.text = date;
        selector.add(option);
    });
    selector.disabled = false;
}

function loggerProcessCoupling(ob) {
    var Arr = ob;
    var selector = document.getElementById("logCoupling");
    selector.innerHTML = "<option value='none'>Select File:</option>";
    Arr.forEach((couple) => {
        var option = document.createElement("option");
        option.text = couple;
        selector.add(option);
    });
    selector.disabled = false;
}

function loggerProcessAvailableTimeFrames(ob) {
    var st = document.getElementById("logStart").disabled = false;
    var en = document.getElementById("logEnd").disabled = false;
    document.getElementById("logSubmitQuery").disabled = false;
    var Arr = ob;
    var str = "Log suggested timeframes: ";
    Arr.forEach((item) => {
        str += item + ', ';
    });
    document.getElementById("logTimeComment").innerHTML = str;
}

function loggerValidateQuery() {
    var elm_start = document.getElementById("logStart");
    var elm_end = document.getElementById("logEnd");
    var start = timeStringToSeconds(elm_start.value);
    var end = timeStringToSeconds(elm_end.value);
    if (start > end) document.getElementById("logTimeComment").innerHTML = "Invalid time window! (End is earlier than Start)";
    else socketSumbitQuery(elm_start.value, elm_end.value);
}

function loggerWrite(ptsArr) {
    var host_id = document.getElementById("logCoupling").value.split('-')[1].split('.')[0];
    var plat_id = document.getElementById("logCoupling").value.split('-')[0];
    let logTable = new GraphTable(plat_id, plat_id, host_id, false);
    ptsArr.forEach((dot, idx) => {
        logTable.reportArray.push(new Report(host_id, plat_id, dot.ADT_A1SNR, dot.ADT_A2SNR, dot.ADT_A1RSSI, dot.ADT_A2RSSI, dot.GDT_A1SNR, dot.GDT_A2SNR, dot.GDT_A1RSSI, dot.GDT_A2RSSI, dot.comment, dot.dotTime));
    });
    logTable.draw();
}