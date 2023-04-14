import csv

csv_file_path = "filepath"

# öffnet csv datei und liest daten aus
with open(csv_file_path, "r", newline="") as csv_file:
    reader = csv.reader(csv_file, delimiter=";")
    data = list(reader)

# durchläuft zeilen und ersetzt jeweils das , mit einem .
for i in range(len(data)):
    for j in range(len(data[i])):
        # überprüft die Zahlen auf dezimal stellen und ändert dann das Komma zu einem Punkt
        if isinstance(data[i][j], str) and "," in data[i][j] and data[i][j].replace(",", "").replace(".", "").isdigit():
    
            data[i][j] = data[i][j].replace(",", ".")

# neue Daten werden in csv datei geschrieben mit einem , als neuen separator
with open(csv_file_path, "w", newline="") as csv_file:
    writer = csv.writer(csv_file, delimiter=",")
    writer.writerows(data)

for row in data:
    print(row)
print("Die Änderungen wurden erfolgreich in der CSV-Datei gespeichert!")
