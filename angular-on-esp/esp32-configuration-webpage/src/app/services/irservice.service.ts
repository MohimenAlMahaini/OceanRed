import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Subject } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class IRServiceService {
  private requestInterval = 1500;
  private listening = false;

  private irSignals: string[] = [];

  private receivableSubject = new Subject<string>();
  public receivableSignal = this.receivableSubject.asObservable();

  constructor(private http: HttpClient) { }

  getIrSignals(): string[] {
    let result: string[] = [];
    this.irSignals.forEach((value) => result.push(value));
    return result;
  }

  resetIrSignals(): void {
    this.irSignals = [];
  }

  startListening(): void {
    if (!this.listening) {
      this.http.get('start-ir-teachin').subscribe((data) => {
        console.log("Finished");
        console.log(data);
      }, (error) => console.log(error), () => console.log("completed!"));; // Inform ESP32 to start.
      this.listening = true;
      this.startRequestLoop();
    }
  }

  stopListening(): void {
    this.http.get("stop-enocean-teachin").subscribe((data) => {
      console.log("Finished");
      console.log(data);
    }, (error) => console.log(error), () => console.log("completed!")); // Inform ESP32 to stop.
    this.listening = false;
  }

  /*StartRequestLoop is WIP */
  private startRequestLoop(): void {
    if (this.listening) {
      setTimeout(() => {

        if (!this.listening)
          return;

        console.log("Request data...")
        this.http.get('get-ir').subscribe((data) => {

          if (data) {
            let container = data as any;
            if (container.signals) {
              for (let i = 0; i < container.signals.length; i++) {
                let signal = container.signals[i];
                if (signal) {
                  this.irSignals.push(signal);
                  this.receivableSubject.next(signal);
                }
              }
            }
          }

          //Notify observers

          this.startRequestLoop();
        },
          (error) => {
            console.log("Error");
            console.log(error);

            this.startRequestLoop();
          });

      }, this.requestInterval);
    }
  }

}