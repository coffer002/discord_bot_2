FROM python:3.11-slim

WORKDIR /app

ENV PYTHONPATH=/app:/app/math
ENV PYTHONUNBUFFERED=1


RUN apt-get update && apt-get install -y \
    gcc \
    make \
    libc-dev \
    libcairo2-dev \
    libfreetype6-dev \
    pkg-config \
    curl \
    && rm -rf /var/lib/apt/lists/*


COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt


COPY . .


RUN curl -sSL -o welcome/stb_image.h https://raw.githubusercontent.com/nothings/stb/master/stb_image.h


RUN gcc -shared -fPIC -o /usr/local/lib/libmath.so math_tools/math_engine.c -lm
RUN gcc -shared -fPIC -o /usr/local/lib/libwelcome.so welcome/welcome_engine.c $(pkg-config --cflags --libs cairo) -lm

ENV PYTHONPATH=/app

CMD ["python", "src/main.py"]