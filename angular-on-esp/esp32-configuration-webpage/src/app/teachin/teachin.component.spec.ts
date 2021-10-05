import { ComponentFixture, TestBed } from '@angular/core/testing';

import { TeachinComponent } from './teachin.component';

describe('TeachinComponent', () => {
  let component: TeachinComponent;
  let fixture: ComponentFixture<TeachinComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ TeachinComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(TeachinComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
