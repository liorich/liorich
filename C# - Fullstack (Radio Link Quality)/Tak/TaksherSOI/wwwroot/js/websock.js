const socket = new WebSocket("wss://" + location.host + "/ws");
// Connection opened
socket.onopen = function (event) {
    addMsg("Websocket connection established",3);
}

socket.onmessage = function(event) {
    var ob = JSON.parse(event["data"]);
    var type_ob = ob["type"];
    var ob_data = ob[type_ob];

    switch (type_ob) {
        case "Data":
            for (group in ob_data.groups) {
                processUpdates(ob_data.groups[group]);
            }
            break;
        case "MeshReport":
            Groups_vars.forEach((group) => { group.updateDomo(ob_data); });
            break;
        case "Message":
            addMsg(ob_data);
            break;
        case "Timeouts":
            handleTimeouts(ob_data);
            break;
        // Rollback
        case "rollbackDates":
            DBRShowDates(ob_data);
            break;
        case "rollbackNetState":
            DBRLoadNetState(ob_data);
            break;
        // Old Logger
        case "logDates":
            loggerProcessDates(ob_data);
            break;
        case "logCoupling":
            loggerProcessCoupling(ob_data);
            break;
        case "logAvailableTimeFrame":
            loggerProcessAvailableTimeFrames(ob_data);
            break;
        case "logReport":
            loggerWrite(ob["logReport"]);
            break;
        // Configurations
        case "PingerConfig":
            MaintenanceShowConfig(ob_data);
            break;
        case "NameConfig":
            MaintenanceShowNameConfig(ob_data);
            break;

    }
}

socket.onerror = function (event) {
    addMsg("Failed to form websocket connection", 1);
}

socket.onclose = function (event) {
    console.log("Socket was closed");
    addMsg("Websocket connection was ended", 1);
}
// Live functionality
function socketRequestAntChange() {
    var pointer = document.getElementById("DB2_LogFunc_chAnt_Select").value;
    socket.send(JSON.stringify({ "type": "message", "request": "changeTxAnt", "id": pointer.toString() }));
    document.getElementById("DB2_LogFunc_chAnt_prompt").remove();
}

function socketSendComment() {
    var plat_id = document.getElementById("DB2_LogFunc_AddComment_Select").value;
    var com = document.getElementById("DB2_LogFunc_CommentBox").value;
    document.getElementById("DB2_LogFunc_CommentBox").value = "";
    document.getElementById("DB2_LogFunc_CommentBox").placeholder = "Comment added...";
    socket.send(JSON.stringify({ "type": "message", "request": "setComment", "id": plat_id, "comment": com }));
}

function socketSendSetup(dict) {
    socket.send(JSON.stringify({ "type": "message", "request": "saveSetup", "dict": JSON.stringify(Object.fromEntries(dict)) }));
}

// Rollback
function socketRequestRollbackDates() {
    socket.send(JSON.stringify({ "type": "message", "request": "requestRollbackDates" }));
}
function socketRequestRollbackNetState(){
    var date = document.getElementById("DBR_Date_Select").value;
    socket.send(JSON.stringify({ "type": "message", "request": "requestRollbackNetState", "date" : date }));
}

// Old Logger
function socketRequestLogDates() {
    socket.send(JSON.stringify({ "type": "message", "request": "requestLoggerDates"}));
}
function socketRequestCoupling() {
    var date = document.getElementById("logDates").value;
    socket.send(JSON.stringify({ "type": "message", "request": "requestLoggerCoupling", "Date" : date }));
}
function socketRequestAvailableTimeFrame() {
    var date = document.getElementById("logDates").value;
    var file = document.getElementById("logCoupling").value;
    socket.send(JSON.stringify({ "type": "message", "request": "requestAvailableTimeFrame", "Date": date, "File" : file }));
}
function socketSumbitQuery(start, end) {
    var date = document.getElementById("logDates").value;
    var file = document.getElementById("logCoupling").value;
    socket.send(JSON.stringify({ "type": "message", "request": "requestLogs", "Date": date, "File": file, "Start": start, "End": end }));
}

function socketRequestPingerConfig() {
    socket.send(JSON.stringify({ "type": "message", "request": "requestPingerConfig" }));
}
function socketRequestPingerConfigSave(config) {
    socket.send(JSON.stringify({ "type": "message", "request": "requestPingerConfigUpdate", "NewConfig": config }));
}

function socketRequestNameConfig() {
    socket.send(JSON.stringify({ "type": "message", "request": "requestNameConfig" }));
}
function socketRequestNameConfigSave(config) {
    console.log("requested");
    socket.send(JSON.stringify({ "type": "message", "request": "requestNameConfigUpdate", "NewConfig": config }));
}

function timeStringToSeconds(timeStr) {
    var a = timeStr.split(':');
    var seconds = (+a[0]) * 3600 + (+a[1]) * 60 + (+a[2]);
    return seconds;
}