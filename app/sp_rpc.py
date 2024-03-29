import sys
import requests
def close(id):
    data = {'id':str(id)}
    r = requests.get('http://0.0.0.0:8999/exit',params = data)
    print(id,"result:",r.status_code)

def register(id,weight,memory):
    data = {'id':str(id),'weight':str(weight),'memory':str(memory)}
    r = requests.get('http://0.0.0.0:8999/register',params = data)
    print(id,"result:",r.status_code)

def change_weight(id,weight):
    data = {'id':str(id),'weight':str(weight)}
    r = requests.get('http://0.0.0.0:8999/changeweight',params = data)
    print(id,"result:",r.status_code)


func_table={'close':close,'register':register,'change_weight':change_weight}
    
if __name__ == "__main__":
    if func_table.get(sys.argv[1]):
        func_table[sys.argv[1]](*sys.argv[2:])