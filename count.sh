find . -not -path "./build**" -name '*.cpp' -o -name '*.h' | xargs wc -l | grep total
