import { Inject, Injectable } from '@angular/core';
import { HttpClient, HttpParams } from '@angular/common/http';
import { Subject } from 'rxjs';
import { EntryI } from '../entry-i';


@Injectable({
  providedIn: 'root'
})

/*This class sends requests to webserver.h */
export class EntryService {

  private entry: EntryI[] = []; // Array of Entries to hold Entries.
  currentID: number = 0;

  constructor(private http: HttpClient) { }

  getCurrentID(): number {
    return this.currentID;
  }

  getEntries(): Promise<EntryI[]> {
    return new Promise<EntryI[]>((resolve, reject) => {
      this.http.get('/getEntries').subscribe((data) => {
        console.log("Finished");
        console.log(data);
        let result: EntryI[] = [];

        if (Array.isArray(data)) {
          data.forEach(element => {
            let entry = element as EntryI;
            result.push(entry);
            console.log(entry);
          });
        }

        resolve(result);
      }, (error) => reject(error));
    });
  }

  testIR(ir: string) {
    console.log("Test IR Signal = " + ir);
    this.http.post('/testIR', { IRRawData: ir }).subscribe((result) => console.log("Test IR send! " + result));
  }

  /* Post request with functionName, Enocean Information, and IR Raw Data to add an Entry*/
  addEntry(functionName: string, irRawData: string, enocean?: string) {
    console.log("Add Entry: functionName = " + functionName + " irRawData = " + irRawData + " enocean = " + enocean);
    this.http.post('/addEntry', { functionName: functionName, irRawData: irRawData })
      .subscribe((data: any) => {
        console.log(data);
        // this request returns created EntryId
        if (data.id && Number.isInteger(data.id)) { // Validate Returned Code.
          this.currentID = data.id;
        }
      });
  }

  /* This function is used to add and REMOVE EnOcean Signals */
  addEnOcean(id: number, enoceanInfo: string) {
    console.log("ADD / Remove EnOcean : " + id + "EnOcean Info = " + enoceanInfo);
    this.http.post('/addEnocean', { id: id, enocean: enoceanInfo }).subscribe((result) => {
      console.log(result);
    });
  }

  /*This function removes an Enocean Signal from an Entry*/
  removeEnocean(entryId: string, enoceanSignal: string): Promise<boolean> {
    return new Promise<boolean>((resolve, reject) => {
      console.log("Removing EnOcean Signal : " + enoceanSignal + " From Entry # : " + entryId);
      this.http.delete('/removeEnocean?entryId=' + entryId + '&enocean=' + enoceanSignal).subscribe((res) => {
        console.log(res);
        resolve(true);
      }, (error) => reject(error));
    });
  }

  /*This function takes an EntryId as a parameter and sends a request to remove the Entry from backend*/
  removeEntry(entryId: string) {
    this.http.delete('/removeEntry?entryId=' + entryId).subscribe((result) => {
      console.log(result);
    });

  }

  //TODO REFRESH PAGE
  save() {
    console.log("Save");
    this.http.get('/save').subscribe((response) => console.log(response));
  }


  /*this function restarts the ESP32*/
  restart() {
    console.log("Restarting ESP . . .");
    this.http.get('/restart');
  }


}


/* @DEPRECATED Post request to edit an Entry*/ // THIS CAN BE REMOVED FROM C++ AND ANGULAR
/*
editEntry(entryId: string, irRawData: string, functionName: string, enoceanTelegram?: string,) {
  console.log("Edit Entry: " + entryId);
  let body = new HttpParams();
  body = body.set('name', functionName);
  body = body.set('ir', irRawData);
  body = body.set('entryId', entryId);
  if (enoceanTelegram) {
    body = body.set('enocean', enoceanTelegram);
  }
  this.http.post('/editEntry', body);
}
*/