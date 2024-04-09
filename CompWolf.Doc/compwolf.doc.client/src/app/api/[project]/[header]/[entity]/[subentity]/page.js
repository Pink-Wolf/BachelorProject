import { getEntity } from "@/lib/api/Entity";
import FunctionViewer from "@/lib/api/FunctionViewer";
import VariableViewer from "@/lib/api/VariableViewer";
import BaseEntityViewer from "@/lib/api/BaseEntityViewer";

export default async function SubEntityPage({ params }) {
    const owner = await getEntity(params.project, params.header, params.entity)
    const target = params.subentity

    var data = undefined
    var isFunction = true
    if (target == params.entity) {
        data = owner.constructor
    }
    if (data === undefined) {
        data = owner.dataMembers?.find((i) => {
            return i.name === target
        })
        if (data !== undefined) isFunction = false
    }
    if (data === undefined) {
        owner.methodGroups?.find((i) => {
            i.items.find((j) => {
                if (j.name === target) {
                    data = j
                    isFunction = true
                }

                return (data !== undefined)
            })
            return (data !== undefined)
        })
    }

    data = {
        ...data,
        project: owner.project,
        header: owner.header,
        owner: owner.name,
        namespace: owner.namespace,
        name: target,
    }

    if (data === undefined) return <BaseEntityViewer />
    else if (isFunction) return <FunctionViewer data={data} />
    else return <VariableViewer data={data} />
}
