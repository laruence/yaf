/*
 * {&$APP_NAME&} — shared stylesheet.
 * Served straight from public/css/ by the web server (the rewrite
 * rules only forward requests that do not match a real file).
 */

:root {
  --yaf-blue:   #2c3e50;
  --yaf-accent: #e74c3c;
  --yaf-bg:     #f4f6f8;
  --yaf-card:   #ffffff;
  --yaf-text:   #34495e;
  --yaf-muted:  #7f8c8d;
}

* { box-sizing: border-box; }

body {
  margin: 0;
  font-family: -apple-system, "Segoe UI", "Helvetica Neue", Arial, sans-serif;
  background: var(--yaf-bg);
  color: var(--yaf-text);
}

header.top {
  background: var(--yaf-blue);
  color: #fff;
  padding: 18px 32px;
  display: flex;
  align-items: center;
  gap: 16px;
}

header.top img.logo {
  height: 56px;
  background: #fff;
  border-radius: 6px;
  padding: 4px;
}

header.top h1 {
  font-size: 22px;
  margin: 0;
  font-weight: 600;
}

header.top .tagline {
  color: #bdc3c7;
  font-size: 13px;
}

nav.top {
  background: #34495e;
  padding: 0 32px;
}

nav.top a {
  display: inline-block;
  color: #ecf0f1;
  text-decoration: none;
  padding: 10px 18px;
  font-size: 14px;
}

nav.top a:hover,
nav.top a.active {
  background: var(--yaf-accent);
  color: #fff;
}

main.content {
  max-width: 860px;
  margin: 32px auto;
  padding: 0 16px;
}

.card {
  background: var(--yaf-card);
  border-radius: 8px;
  box-shadow: 0 1px 4px rgba(0, 0, 0, .12);
  padding: 28px 32px;
  margin-bottom: 24px;
}

.card h2 {
  margin-top: 0;
  color: var(--yaf-blue);
  border-bottom: 2px solid var(--yaf-accent);
  padding-bottom: 8px;
}

.card .muted { color: var(--yaf-muted); font-size: 13px; }

table.records {
  width: 100%;
  border-collapse: collapse;
  margin-top: 12px;
}

table.records th,
table.records td {
  text-align: left;
  padding: 10px 14px;
  border-bottom: 1px solid #e3e7ea;
  font-size: 14px;
}

table.records th {
  background: var(--yaf-bg);
  color: var(--yaf-blue);
}

table.records tr:hover td { background: #fbfcfd; }

button#visit {
  margin-top: 16px;
  background: var(--yaf-accent);
  color: #fff;
  border: 0;
  border-radius: 5px;
  padding: 10px 22px;
  font-size: 14px;
  cursor: pointer;
}

button#visit:hover { background: #c0392b; }

footer.bottom {
  text-align: center;
  color: var(--yaf-muted);
  font-size: 12px;
  padding: 24px 0 40px;
}
