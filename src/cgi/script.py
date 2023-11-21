#!/usr/bin/env python3
import sys
import requests

def get_weather(city, api_key):
    api_url = f"http://api.openweathermap.org/data/2.5/weather?q={city}&appid={api_key}&units=metric"
    try:
        response = requests.get(api_url)
        if response.status_code == 200:
            data = response.json()
            temp = data['main']['temp']
            weather = data['weather'][0]['description']
            return temp, weather
        else:
            return None, None
    except requests.RequestException:
        return None, None

def generate_html(city, temp, weather):
    """Génère une page HTML avec les informations météorologiques."""
    return f"""<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Météo pour {city}</title>
</head>
<body>
    <h1>Météo pour {city}</h1>
    <p>Température : {temp} °C</p>
    <p>Conditions : {weather}</p>
</body>
</html>"""

def generate_error_html(city):
    return f"""<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Météo non trouvée</title>
</head>
<body>
    <h1>Météo non disponible</h1>
    <p>La météo pour la ville "{city}" n'a pas pu être trouvée.</p>
</body>
</html>"""

def main():
    if len(sys.argv) > 1:
        city = sys.argv[1]
        api_key = '7392c8ae76e2e24886bb76c348d72460'

        temp, weather = get_weather(city, api_key)
        if temp is not None and weather is not None:
            html_content = generate_html(city, temp, weather)
        else:
            html_content = generate_error_html(city)
        print(html_content)
    else:
        print("<p>Veuillez fournir le nom d'une ville.</p>")

if __name__ == "__main__":
    main()
