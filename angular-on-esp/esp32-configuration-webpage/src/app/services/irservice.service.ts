import { Injectable, isDevMode } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Subject } from 'rxjs';

const devIP = "http://192.168.124.21:81";
// const devIP = "http://192.168.0.242:81";
const IP = isDevMode() ? devIP : '';
// const IP = '';

@Injectable({
  providedIn: 'root'
})
export class IRServiceService
{
  private requestInterval = 1500;
  private listening = false;

  private irSignals: string[] = [];

  private receivableSubject = new Subject<string>();
  public receivableSignal = this.receivableSubject.asObservable();

  constructor(private http: HttpClient) { }

  getIrSignals(): string[]
  {
    let result: string[] = [];
    this.irSignals.forEach((value) => result.push(value));
    return result;
  }

  resetIrSignals(): void
  {
    this.irSignals = [];
  }

  startListening(): void
  {
    if (!this.listening)
    {
      this.http.get(IP + '/start-ir-teachin').subscribe((data) =>
      {
        console.log("Start IR Teachin");
        console.log(data);
      }, (error) => console.log(error), () => console.log("completed!"));; // Inform ESP32 to start.
      this.listening = true;
      this.startRequestLoop();
    }
  }

  /*StartRequestLoop */
  private startRequestLoop(): void
  {
    if (this.listening)
    {
      setTimeout(() =>
      {

        if (!this.listening)
          return;

        console.log("Request IR data...")
        this.http.get(IP + '/get-ir').subscribe((data) =>
        {

          if (data)
          {
            let container = data as any;
            if (container.signals)
            {
              for (let i = 0; i < container.signals.length; i++)
              {
                let signal = container.signals[i];
                if (signal)
                {
                  this.irSignals.push(signal);
                  this.receivableSubject.next(signal);
                }
              }
            }
          }

          //Notify observers

          this.startRequestLoop();
        },
          (error) =>
          {
            console.log("Error");
            console.log(error);

            this.startRequestLoop();
          });

      }, this.requestInterval);
    }
  }

  stopListening(): void
  {
    this.http.get(IP + "/stop-ir-teachin").subscribe((data) =>
    {
      console.log("Stoped IR Teachin");
      console.log(data);
    }, (error) => console.log(error), () => console.log("completed!")); // Inform ESP32 to stop.
    this.listening = false;
  }



}