import requests
import threading
from concurrent.futures import ThreadPoolExecutor, as_completed

def send_request(session, url):
    try:
        response = session.get(url, timeout=1)
        print(f"Status: {response.status_code}")
    except requests.RequestException as e:
        print(f"Fout: {e}")

def send_requests_batch(url, total_requests, max_threads=50):
    with ThreadPoolExecutor(max_workers=max_threads) as executor:
        with requests.Session() as session:
            futures = [executor.submit(send_request, session, url) for _ in range(total_requests)]

            for future in as_completed(futures):
                pass  # All output is handled in send_request

if __name__ == "__main__":
    send_requests_batch("http://localhost:8080", total_requests=10_000, max_threads=100)
