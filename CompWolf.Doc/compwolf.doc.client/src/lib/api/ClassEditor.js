'use client'

import { useState } from "react";
import Input from "@/lib/Input";
import { postEntity } from "./Entity";
import { useRouter } from "next/navigation";
export default function ClassEditor(props) {
    const [data, setData] = useState({
        type: "class",
        project: props.projects[0],
        header: props.projects[0].headers[0],
    })
    const [processing, setProcessing] = useState(false)

    const router = useRouter()

    function onSubmit(event) {
        setProcessing(true)
        event.preventDefault();

        const output = {
            ...data,
            project: data.project.name,
            header: data.header.name,
        }
        console.log(output)

        postEntity(output).then(result => {
            if (!result) {
                setProcessing(false)
                alert("Could not create class")
            } else {
                router.push(`api/${output.project}/${output.header}/${output.name}`)
            }
        })
    }

    if (processing) return <p>Created class...</p>

    return (
        <form onSubmit={onSubmit}>
            <Input field="name" setter={setData} value={data} />
            <Input field="declaration" setter={setData} value={data} />
            <section>Description:
                <ul>
                    <li> <Input field="briefDescription" label="Brief:" type="textarea" setter={setData} value={data} /> </li>
                    <li> <Input field="detailedDescription" label="Detailed:" type="textarea" setter={setData} value={data} /> </li>
                </ul>
            </section>

            <Input field="project" type="select" options={props.projects.map(x => x.name)} setter={(x) => setData({
                ...x,
                project: props.projects.find(y => y.name === x.project),
                header: props.projects.find(y => y.name === x.project).headers[0]
            })} value={data} />
            <Input field="headers" type="select" options={data.project.headers} setter={(x) => setData({ ...x, header: "" })} value={data} />
            <Input field="namespace" setter={setData} value={data} />

            <Input field="warnings" type="array" setter={setData} value={data}
                forEach={(x, i, setter) => {
                    return (
                        <textarea value={x} onChange={(e) => { setter(e.target.value) }} />
                    )
                }}
            />

            <Input field="dataMembers" type="array" setter={setData} value={data}
                forEach={(x, i, setter) => {
                    return (
                        <section>
                            <Input field="name" setter={setter} value={x} />
                            <Input field="declaration" setter={setter} value={x} />
                            <section>Description:
                                <ul>
                                    <li> <Input field="briefDescription" label="Brief:" type="textarea" setter={setter} value={x} /> </li>
                                    <li> <Input field="detailedDescription" label="Detailed:" type="textarea" setter={setter} value={x} /> </li>
                                </ul>
                            </section>
                            <Input field="related" type="array" setter={setter} value={x}
                                forEach={(x, i, setter) => {
                                    return (
                                        <input type="text" value={x} onChange={(e) => { setter(e.target.value) }} />
                                    )
                                }}
                            />
                        </section>
                    )
                }}
            />

            <Input field="constructor" type="container" setter={setData} value={data} inputs={(x, setter) => {
                return <ul>
                    <li>
                        <Input field="warnings" type="array" setter={setter} value={x}
                            forEach={(x, i, setter) => {
                                return (
                                    <textarea value={x} onChange={(e) => { setter(e.target.value) }} />
                                )
                            }}
                        />
                    </li>
                    <li>
                        <Input field="overloads" type="array" setter={setter} value={x}
                            forEach={(overload, i, overloadSetter) => {
                                return (
                                    <section>
                                        <Input field="declaration" setter={overloadSetter} value={overload} />
                                        <Input field="description" type="textarea" setter={overloadSetter} value={overload} />
                                    </section>
                                )
                            }}
                        />
                    </li>
                    <li>
                        <Input field="related" type="array" setter={setter} value={x}
                            forEach={(x, i, setter) => {
                                return (
                                    <input type="text" value={x} onChange={(e) => { setter(e.target.value) }} />
                                )
                            }}
                        />
                    </li>
                </ul>
            }} />
            <Input field="methodGroups" type="array" setter={setData} value={data}
                forEach={(group, i, groupSetter) => {
                    return (
                        <section>
                            <Input field="name" setter={groupSetter} value={group} />
                            <Input field="description" type="textarea" setter={groupSetter} value={group} />
                            <Input field="items" label="methods" type="array" setter={groupSetter} value={group}
                                forEach={(method, i, methodSetter) => {
                                    return (
                                        <section>
                                            <Input field="name" setter={methodSetter} value={method} />
                                            <Input field="briefDescription" type="textarea" setter={methodSetter} value={method} />
                                            <Input field="warnings" type="array" setter={methodSetter} value={method}
                                                forEach={(x, i, setter) => {
                                                    return (
                                                        <textarea value={x} onChange={(e) => { setter(e.target.value) }} />
                                                    )
                                                }}
                                            />
                                            <Input field="overloads" type="array" setter={methodSetter} value={method}
                                                forEach={(overload, i, overloadSetter) => {
                                                    return (
                                                        <section>
                                                            <Input field="declaration" setter={overloadSetter} value={overload} />
                                                            <Input field="description" type="textarea" setter={overloadSetter} value={overload} />
                                                        </section>
                                                    )
                                                }}
                                            />
                                            <Input field="related" type="array" setter={methodSetter} value={method}
                                                forEach={(x, i, setter) => {
                                                    return (
                                                        <input type="text" value={x} onChange={(e) => { setter(e.target.value) }} />
                                                    )
                                                }}
                                            />
                                        </section>
                                    )
                                }}
                            />
                        </section>
                    )
                }}
            />

            <Input field="related" type="array" setter={setData} value={data}
                forEach={(x, i, setter) => {
                    return (
                        <input type="text" value={x} onChange={(e) => { setter(e.target.value) }} />
                    )
                }}
            />

            <Input field="example" type="container" setter={setData} value={data} inputs={(x, setter) => {
                return <ul>
                    <li>
                        <Input field="description" type="textarea" setter={setter} value={x} />
                    </li>
                    <li>
                        <Input field="code" type="textarea" setter={setter} value={x} />
                    </li>
                </ul>
            }} />

            <input type="submit" value="Create" />
        </form>
    );
}
