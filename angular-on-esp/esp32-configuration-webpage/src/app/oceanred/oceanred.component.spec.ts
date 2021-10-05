import { ComponentFixture, TestBed } from '@angular/core/testing';

import { OceanredComponent } from './oceanred.component';

describe('OceanredComponent', () => {
  let component: OceanredComponent;
  let fixture: ComponentFixture<OceanredComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ OceanredComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(OceanredComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
