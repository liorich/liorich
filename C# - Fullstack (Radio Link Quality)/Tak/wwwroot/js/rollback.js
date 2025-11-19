function DBRShowDates(ob_data) {
    var Arr = ob_data;
    var selector = document.getElementById("DBR_Date_Select");
    selector.innerHTML = "<option value='none'>...</option>";
    Arr.forEach((date) => {
        var option = document.createElement("option");
        option.text = date;
        selector.add(option);
    });
    selector.disabled = false;
}
function DBRLoadNetState(ob_data) {
    console.log(ob_data);
}