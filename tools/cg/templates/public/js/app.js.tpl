/*
 * {&$APP_NAME&} — shared page script.
 * Served straight from public/js/ by the web server (the rewrite
 * rules only forward requests that do not match a real file).
 *
 * Loaded with defer on every page; demonstrates wiring a little
 * client-side behaviour into the skeleton.
 */
(function () {
    "use strict";

    /* Highlight the nav entry that matches the current path. */
    document.querySelectorAll("nav.top a").forEach(function (a) {
        var href = a.getAttribute("href");
        if ((href === "/" && location.pathname === "/") ||
            (href !== "/" && location.pathname.indexOf(href) === 0)) {
            a.classList.add("active");
        }
    });

    /* The user page carries a "visit user" button: jump to
     * /index/index/user/id/:id with a random id. */
    var btn = document.getElementById("visit");
    if (btn) {
        btn.addEventListener("click", function () {
            location.href = "/index/index/user/id/" + (1 + Math.floor(Math.random() * 5));
        });
    }
})();
