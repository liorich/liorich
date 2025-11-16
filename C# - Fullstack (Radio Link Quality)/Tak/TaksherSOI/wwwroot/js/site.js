// Please see documentation at https://docs.microsoft.com/aspnet/core/client-side/bundling-and-minification
// for details on configuring this project to bundle and minify static web assets.

// Write your JavaScript code.

// Main

var timer = setInterval(DateAndTime, 1000);

let com_elm = document.getElementById("CommentContainer");

function ClearLines() {
    Lines.forEach((line_name) => {
        var line_elm = document.getElementById(line_name);
        if (line_elm != null) line_elm.remove();
    });
}

// Local time
function DateAndTime() {
    var e = document.getElementById("DB2_System_Time");
    var currentdate = new Date();
    var datetime = "System time: " + currentdate.getDate() + "/"
        + (currentdate.getMonth() + 1) + "/"
        + currentdate.getFullYear() + " "
        + ('0' + currentdate.getHours()).slice(-2) + ":"
        + ('0' + currentdate.getMinutes()).slice(-2) + ":"
        + ('0' + currentdate.getSeconds()).slice(-2);
    e.innerHTML = datetime;
}

function getTime() {
    var currentdate = new Date();
    var datetime = ""
        + ('0' + currentdate.getHours()).slice(-2) + ":"
        + ('0' + currentdate.getMinutes()).slice(-2) + ":"
        + ('0' + currentdate.getSeconds()).slice(-2);
    return datetime;
}

// Sidebar light up

function litUpSidebar(e) {
    e.style.backgroundColor = "#5175ae";
}

function delay(time) {
    return new Promise(resolve => setTimeout(resolve, time));
}

function hideComment() {
    com_elm.style.visibility = "hidden";
}
function MaintenanceShowConfig(config) {
    var text = document.getElementById("PingerConfigView");
    text.value = config;
    text.hidden = false;
    text.style.height = (text.scrollHeight + 50) + "px";
    document.getElementById("Maintenace_SavePingerConfig").disabled = false;
}
function MaintenanceSavePingerConfig() {
    var text = document.getElementById("PingerConfigView");
    var config = text.value;
    text.hidden = true;
    document.getElementById("Maintenace_SavePingerConfig").disabled = true;
    socketRequestPingerConfigSave(config);
}
function MaintenanceShowNameConfig(config) {
    var text = document.getElementById("NameConfigView");
    text.value = config;
    text.hidden = false;
    text.style.height = (text.scrollHeight + 50) + "px";
    document.getElementById("Maintenace_SaveNameConfig").disabled = false;
}
function MaintenanceSaveNameConfig() {
    var text = document.getElementById("NameConfigView");
    var config = text.value;
    text.hidden = true;
    document.getElementById("Maintenace_SaveNameConfig").disabled = true;
    socketRequestNameConfigSave(config);
    Groups_vars.forEach((group) => {
        group.removeSelf();
    });
    Groups_vars = [];
    Groups_ids = [];
}