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

    /* The index page carries a "visit user" button: jump to the custom
     * rewrite route /user/:id with a random id, proving the route
     * works end to end. */
    var btn = document.getElementById("visit");
    if (btn) {
        btn.addEventListener("click", function () {
            location.href = "/user/" + (1 + Math.floor(Math.random() * 5));
        });
    }
})();
