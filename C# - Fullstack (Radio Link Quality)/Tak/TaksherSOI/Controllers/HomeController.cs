using Microsoft.AspNetCore.Authentication;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using System.Security.Claims;

namespace Tak.Controllers
{
    public class HomeController : Controller
    {
        [AllowAnonymous]
        public IActionResult Login()
        {
            return View();
        }

        [HttpPost]
        [AllowAnonymous]
        public async Task<IActionResult>Login(string username, string password)
        {
            if (username == "admin" && password == "TAK!")
            {
                var claims = new List<Claim> { new Claim(ClaimTypes.Name, username) };
                var identity = new ClaimsIdentity(claims, "CookieAuth");
                var principal = new ClaimsPrincipal(identity);

                await HttpContext.SignInAsync("CookieAuth", principal);
                return RedirectToAction("Index", "Home");
            }
            ViewBag.Message = "Invalid Credentials";
            return View();
        }
        public async Task<IActionResult> Logout()
        {
            await HttpContext.SignOutAsync("CookieAuth");
            return RedirectToAction("Index", "Home");
        }

        public IActionResult Index()
        {
            return View();
        }
        public IActionResult Home()
        {
            return Index();
        }

        public IActionResult Info()
        {
            return View();
        }
        public IActionResult Logs()
        {
            return View();
        }
        public IActionResult Maintenance()
        {
            return View();
        }
        public IActionResult Rollback()
        {
            return View();
        }
        [Authorize]
        public IActionResult Admin()
        {
            return View();
        }
        public IActionResult Setup()
        {
            return View();
        }
    }
}
