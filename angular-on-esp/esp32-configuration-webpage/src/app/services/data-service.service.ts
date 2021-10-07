import { Injectable } from '@angular/core';
import { ActivatedRoute } from '@angular/router';
import { IenoceanData } from '../ienocean-data';
import { Observable, Subject } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class DataServiceService {

  private _enOceanData: IenoceanData[] = [];
  test!: string;

  addSignal(signal: IenoceanData) {
    this._enOceanData.push(signal);
    console.log("HELLO WORLD! FROM SERVICE");
    console.log(signal)
  }

  getSignals(): IenoceanData[] {
    console.log("GETTING SIGNAL FROM SERVICE !");
    console.log(this._enOceanData)
    return this._enOceanData;
  }

  addTest(string: string) {
    this.test = string;
    console.log(this.test);
  }

  getTest() {
    return this.test;
  }

  p = ''
  s = ''

  constructor(private activatedRoute: ActivatedRoute) {

  }

}
