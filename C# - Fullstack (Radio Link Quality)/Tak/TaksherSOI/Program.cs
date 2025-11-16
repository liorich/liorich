using Microsoft.AspNetCore.Authentication;
using Microsoft.AspNetCore.Mvc;
using System.Net.Http.Headers;
using System.Runtime.CompilerServices;
using Tak.Globals;
using Tak.Models;
using static Tak.Models.Echoer;

var builder = WebApplication.CreateBuilder(args);

// Our Data feeder and logger
var WorkerMaster = new Worker();

// Add services to the container.
builder.Services.AddRazorPages();
//builder.Services.AddSignalR();

builder.Services.AddAuthentication("CookieAuth").AddCookie("CookieAuth", options =>
{
    options.Cookie.Name = "CookieAuth";
    options.LoginPath = "/Home/Login";
});

var app = builder.Build();



// Configure the HTTP request pipeline.
if (!app.Environment.IsDevelopment())
{
    app.UseExceptionHandler("/Error");
    // The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
    app.UseHsts();
}

app.UseHttpsRedirection();
app.UseStaticFiles();

app.UseRouting();
app.UseAuthentication();
app.UseAuthorization();
app.MapRazorPages();

app.UseWebSockets();

//app.MapHub<ChatHub>("/chat-hub");

app.Use(async (context, next) =>
{
    if (context.Request.Path == "/ws")
    {
        if (context.WebSockets.IsWebSocketRequest)
        {
            using var webSocket = await context.WebSockets.AcceptWebSocketAsync();
            await Echo(webSocket, WorkerMaster);
        }
        else
        {
            context.Response.StatusCode = StatusCodes.Status400BadRequest;
        }
    }
    else
    {
        await next(context);
    }
});

// Routing Table

app.MapGet("/", ctx =>
{
    ctx.Response.Redirect("/Home");
    return Task.CompletedTask;
});

app.MapGet("/admin", ctx =>
{
    ctx.Response.Redirect("/Home/Admin");
    return Task.CompletedTask;
});

app.MapControllerRoute(
    name: "default",
    pattern: "{controller=Home}/{action=Index}/{id?}");

app.Run();
// app.Run("http://*:5000");

