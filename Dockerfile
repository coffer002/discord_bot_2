FROM python:3.11-slim

WORKDIR /app

ENV PYTHONPATH=/app:/app/math

# Instala dependências do sistema para compilar C
RUN apt-get update && apt-get install -y gcc make libc-dev && rm -rf /var/lib/apt/lists/*

# Copia e instala dependências do Python
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copia o código-fonte
COPY . .

# Compila a biblioteca C para dentro da pasta src/math/
RUN gcc -shared -fPIC -o /usr/local/lib/libmath.so math/math_engine.c -lm

CMD ["python", "src/main.py"]
